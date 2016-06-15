"# truetypefontparsetoy" 

读取ttf ttc文件,解析里面的glyph的x,y坐标,并且画出轮廓.本来打算完善,
但是能找到的资料太少,未能完成ttf readtime auto hint,以及光栅化.

函数都揉成一坨了,内存根本没管,解析flag,x,y,以及后面的对连续的off curve点进行差值各自
用了一个循环,其实可以放在一个循环里,但是会更复杂.



有用的链接:
https://freddie.witherden.org/pages/font-rasterisation/
https://www.freetype.org/autohinting/hinter.html


Real-Time Grid Fitting of Typographic Outlines的一部分翻译
http://www.tug.org/TUGboat/tb24-3/lemberg.pdf


* edges are the basis of grid-alignement and point interpolation in the FreeType Auto-Hinter.

        a  closed curve
             |
             v
outline -> contour -> edge -> segments -> points
                                 |
                                 v
                                stem(两个segment)
1. 划分segment
segment:
是近似对齐于某个坐标轴的一系列点.允许的和直线的最大偏差是1/12(4.7°)--这是一个启发值.
至少有两个点(除非...)
每个segment有某个维度的一个主方向的坐标和另外一个维度的坐标,确定她的极值(extrema)--min_coord或者max_coord

2. 把segments划分成边(edges)
edge是主维度上一个单独位置上一致的一个或多个segment.

auto-hinter 先尝试grid fit edge,然后对齐edge上的segments,除非它检测到它们从衬线那来的.

segment需要被连接(linked to)到其segment的上面,以期检测(detect)stem.两个相反方向并且彼此足够接近的segment构成stem.
垂直方向的stem有下面两种情况:
+ TrueType fonts里,最左边的segment向上,最右边的向下.
+ Type 1,方向相反.

连接segment的是贪婪算法.它尝试从相反方向的segment里找出"最近"的."最近"意味着segments在"另一个"维度"足够对齐(aligned),并且在主方向(main one)上接近.
每个segment最多有一个"最好"的组成black distance的候选segment,或者一个都没有.
注意,两个不同的segment可能有同样的候选,这通常意味着一个衬线.

segment分为圆(round)和方(flat).如果它的极值(可能是一个单独的点或者一系列的点)的前一个和后一个点都是控制点,那么是圆的.否则是方的.

圆和方的segments被收集起来,然后平均值被用来定义蓝色区域(blue zones)和上冲值(overshoot values).


edges是用来收集远着主维度坐标轴的,有三种类型:
+ Free edge: 没有连接到其他edge的
+ 包含至少一个stem的segment
+ 衬线edge:仅包含一个或多个衬线segment

最后,edge能够连接到blue zones,取决于他们的位置和它们包含的segment的朝向.

* Grid Fitting
到现在为止收集的数据可以用来移动glyph的轮廓的点,到可以改善特定输出设备分辨率的整体形状的位置.
这个复杂的过程分成两步:

+ Globally
  每次x y的分辨率改变了,全局上重新缩放和fitted.这允许我们当太小或太大的时候停止激活某些blue
  zones

+ Edge Grid Fitting
  首先,边被fit到像素格子:
   1. 垂直维度,所有连接到了blue zones被对齐到zone的位置(叫做 blue edges).这个包成了glyph高度
     的连续性.
   2. stem edge被fit到像素格子.stem的宽 和 位置是被或多或少的独立计算出来的.
      如果stem edge连接到blue edge,它的位置直接计算.否则微妙的对齐可能发生,这超过我们的讨论范
      围.
   3. 最后,衬线edge被对齐.

计算stem的位置和大小对于微调很敏感,包含了大量的启发式的常量.大多数auto-hinter的patches是针对
相关例程(routine)的改进.

+ Segment Grid Fitting
 在对齐了所有edges之后,对应的segments被fit到同样的位置.这强制这些segments上的点接受edge的
 位置.  


+ Strong Points
  经验表明,不是edge一部分的点需要被插值到在它们最接近的两个edges中间,尽管它们不是这些特定points
  的contour的一部分.典型的候选的点是:
  - angle point: in 和 out direction 非常不同
  - inflection point: 弯曲点,in 和 out direction相同,但是曲率的符号不同.

+ Weak Points
  outline上的其他必须用它们前一个和后一个没有fitted的contour邻居的坐标插值计算的点,它们叫
 weak point.
