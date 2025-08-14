# miniCMD

C99 实现解析文本并调用对应函数

- 命令的形式与C语言调用函数的形式一致 `fun(1, 2, 3);` ，像C语言一样支持空格和换行，末尾的 `;` 可省略

- 一个函数最多支持20个参数，如果需要支持更多参数则需修改代码，但永远都只能支持有限个参数
- 支持输入整数和小数（包括负数）
- 自动转换参数为函数接受的类型，如果函数参数接受一个整数，那么即使输入一个小数也会自动转为整数。
- 函数返回值会自动以浮点数或整数打印。

## 效果展示

有如下函数：

```c
void fun1(int a, uint8_t b, float c)
{
    printf("fun1: %d, %d, %f\n", a, b, c);
}
float fun2(float a, int b, int8_t c)
{
    printf("fun2: %f, %d, %d\n", a, b, c);
    return a;
}
int fun3(void)
{
    printf("fun3\n");
    return 114514;
}
```

测试代码为：

```c
/* 进行命令函数注册 */
MAKE_CMD(void, fun1, int, uint8_t, float);
MAKE_CMD(float, fun2, float, int, int8_t);
MAKE_CMD(int, fun3, void);

/* 通过字符串调用函数 */
char* test_list[]={
        "fun1(1,2,3)",
        "fun2(1.1, 2.2, 3.3)",
        " fun3  ( ) ;  "};
for (int i=0; i<4; ++i)
{
    printf("\n%s\n",test_list[i]);
    miniCMD_run(test_list[i]);
}
```

输出结果为：

```c
fun1(1,2,3)
>>(1 2 3 )--->void fun1(int, uint8_t, float)
fun1: 1, 2, 3.000000
return<<

fun2(1.1, 2.2, 3.3)
>>(1.100000 2.200000 3.300000 )--->float fun2(float, int, int8_t)
fun2: 1.100000, 2, 3
return 1.100000<<

 fun3  ( ) ;
>>()--->int fun3(void)
fun3
return 114514<<
```

## 文件说明

本项目有4个文件，包括1个 `.c` 文件和3个 `.h` 文件：

- `miniCMD.c` ：实现文本解码和函数调用
- `miniCMD.h` ：是 `miniCMD.c` 对外提供的接口，只有一个初始化函数和一个解析函数
- `miniCMD_makeCmd.h` ：提供一个宏 `MAKE_CMD(ret, fun_name, ...)` 用于添加指定函数作为命令，添加命令只会在编译时添加。这个宏只是定义了一个变量和一个代理函数，所以不能在函数体内使用此宏。理论上可以用于任何文件中，但是依然不建议这么做，因为这个头文件会引入很多没用的宏，建议在 `miniCMD_config.h` 中使用 。
- `miniCMD_config.h` ：移植时需要修改文件中的 `CMD_print(...)` 宏以支持打印解码信息（见 [使用说明](#使用说明) ），除此之外没有其他东西了。这个文件的目的是提供一个使用 `MAKE_CMD()` 的空间，原因正如上面所说

## 使用说明

只需要关注下面的函数：

```c
void miniCMD_init(void);
int miniCMD_run(const char *str);
int miniCMD_run_s(char *str, unsigned int size);
#define MAKE_CMD(ret_type, name, ...)
```

`miniCMD_init()` 只需要在 `miniCMD_run()` 之前调用即可

`miniCMD_run()` 对传入的字符串进行解码，查找匹配由 `MAKE_CMD` 创建的命令，并执行对应函数

`miniCMD_run_s()` 如果无法保证传入的字符串是以 `\0` 结尾，就需要用这个函数在指定位置添加 `\0` 后运行 `miniCMD_run` 

`MAKE_CMD(ret_type, name, ...)` 这个宏用于添加指令，不能在函数体内调用此宏。以下是参数的说明：

- `ret_type` 函数返回值的类型，如果为 `void` 则不会打印函数的返回值，如果为 `float` 则以浮点数 `%f` 打印返回值，如果为其它类型则整数 `%d` 打印返回值
- `name` 要添加的函数名
- `...` 函数的参数类型，可以类似于这样 `int,float,int`  ，如果函数没有参数，则需填写 `void` 或者无需填写

示例 `MAKE_CMD(float, fun, int, float)`

## 移植说明

根据需要，修改 `miniCMD_config.h` 中的 `CMD_print(...)` 即可，这个宏用于 miniCMD 打印解析结果

以下是两个修改示例：

- 通过控制台打印

  ```c
  #include "stdio.h"
  #define CMD_print(...) printf(__VA_ARGS__)
  ```

- 通过串口打印

  ```c
  #include "uart.h"
  #define CMD_print(...) uart_print(__VA_ARGS__)
  ```


目前只支持 `GNU` 和 `ARM_CC` 编译器，对于不支持的编译器，只需要修改6行代码。如果使用其它编译器，`#error` 宏会生效，根据报错的说明进行更改即可。

