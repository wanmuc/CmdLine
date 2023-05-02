# 1.CmdLine
> 使用c++实现的命令行参数解析，接口和go的flag包类型，接口简单易懂且易用。

# 2.提供的api
- BoolOpt、Int64Opt和StrOpt函数用于设置可选选项，并支持设置默认值。
- Int64OptRequired和StrOptRequired函数用于设置必设选项，无法指定默认值。
- SetUsage函数用于设置输出命令使用说明的函数。
- Parse函数用于执行命令行参数的解析，无返回值，如果解析失败则直接退出程序。

# 3.如何使用
- 首先，调用第2点中提到的函数，用于设置对选项的要求。
- 其次，调用SetUsage函数，设置输出命令使用说明的函数。
- 最后，调用Parse函数执行命令行参数的解析。

具体的使用，可以查看示例文件demo.cpp。

# 4.微信公众号
欢迎关注微信公众号「Linux后端研发工程实践」，第一时间获取最新文章！扫码即可订阅。也欢迎大家加我个人微信号：wanmuc2018，让我们共同进步。
![img.png](https://github.com/wanmuc/CmdLine/blob/main/mp_account.png#pic_center=660*180)
