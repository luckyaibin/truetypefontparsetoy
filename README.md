"# truetypefontparsetoy" 

读取ttf ttc文件,解析里面的glyph的x,y坐标,并且画出轮廓.本来打算完善,
但是能找到的资料太少,未能完成ttf readtime auto hint,以及光栅化.

函数都揉成一坨了,内存根本没管,解析flag,x,y,以及后面的对连续的off curve点进行差值各自
用了一个循环,其实可以放在一个循环里,但是会更复杂.
