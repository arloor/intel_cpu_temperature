# c语言  读取Intel cpu MSR寄存器，以读取cpu温度

## 编译

```bash
yum groupinstall "Development Tools"
yum install -y zlib-devel # debian系自行修改
gcc main.c -lm -o mo
```

## 运行

运行需要sudo权限！！

运行结果：
```bash
$ sudo ./mo
cpu max temp= 100
cpu temps= 47 47 46 47 47 48 46 47
cpu temps= 47 48 46 47 47 48 46 47
cpu temps= 47 47 46 47 47 47 46 47
cpu temps= 47 47 46 47 47 47 46 47
cpu temps= 47 47 47 48 47 47 47 48
cpu temps= 69 68 68 67 69 68 68 67
cpu temps= 78 75 76 74 78 75 76 74
cpu temps= 50 50 49 50 50 50 49 50
cpu temps= 51 50 49 50 51 50 49 50
cpu temps= 51 57 52 53 51 57 52 53
cpu temps= 79 75 74 75 79 75 74 75
cpu temps= 76 73 74 73 76 73 74 73
cpu temps= 81 79 81 79 81 79 81 79
cpu temps= 84 81 83 81 84 81 83 81
cpu temps= 59 58 58 58 59 58 58 58
cpu temps= 57 57 56 56 57 57 56 56
cpu temps= 56 55 55 55 56 55 55 55
cpu temps= 54 56 54 54 54 56 54 55
cpu temps= 54 54 53 54 54 54 53 54
cpu temps= 53 53 52 53 53 53 52 53
cpu temps= 52 53 51 52 52 53 51 52
cpu temps= 52 52 51 52 52 52 51 52
cpu temps= 51 51 50 51 51 51 50 51
cpu temps= 51 51 50 51 51 50 50 51
cpu temps= 51 50 50 50 51 50 50 50
cpu temps= 50 50 49 50 50 50 49 50
cpu temps= 51 50 50 50 51 50 50 50
cpu temps= 50 50 49 50 50 50 49 50
```

# 目标：

下一步写MSR寄存器，来设置cpu的功耗墙、温度墙。

# c语言语法使用参见

[我的C_study](https://github.com/arloor/C_study)

# python原项目：

[lenovo-throttling-fix](https://github.com/erpalma/lenovo-throttling-fix)
