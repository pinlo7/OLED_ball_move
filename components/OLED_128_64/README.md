## 自定义组件

使用命令:  idf.py -C components create-component <组件名>

在根文件夹下  components 文件夹下自动创建组件

每个文件夹代表一个组件

文件夹下自动创建  CMakeLIsts.txt 文件

如果组件需要依赖其他组件可以添加如下

`REQUIRES driver`  代表依赖 driver组件  编译器自动寻找, 否则 include 包含时会报错 not fould
