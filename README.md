# valkyrEngine

7.23
span储存tuple
tuple用继承 只保留一个Head value，最后t value
获得tuple中第i个元素，tupleidx的类型int i，t…，cpp17的if constexpr
检查可否constexpr（sizeof… T… -1），Rest…递归到0，T
Tuple size同理
T… 参数逗号展开，检查可否直接放函数参数里

tuple在chunk中new：head，rest，每个head new一下，返回指针指向的内存

