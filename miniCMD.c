/**
 * @version 1.0
 * @file miniCMD.c
 * @link https://github.com/EShuoTan/miniCMD
 * @brief 实现字符串解析和命令函数调用
 * @author TanES (tan_163mail@163.com)
 */
#include "miniCMD.h"
#include "miniCMD_config.h"
#include "miniCMD_makeCmd.h"

#if defined(__CC_ARM) || (defined(__ARMCC_VERSION) && __ARMCC_VERSION >= 6000000)
extern const unsigned int miniCMDcommands$$Base;
extern const unsigned int miniCMDcommands$$Limit;
#define CMDS_start ((miniCMD_cmd_t *)(&miniCMDcommands$$Base))
#define CMDS_end ((miniCMD_cmd_t *)(&miniCMDcommands$$Limit))
#elif defined(__GNUC__)
extern size_t __start_miniCMDcommands;
extern size_t __stop_miniCMDcommands;
#define CMDS_start ((miniCMD_cmd_t *)(&__start_miniCMDcommands))
#define CMDS_end ((miniCMD_cmd_t *)(&__stop_miniCMDcommands))
#else
#error "miniCMD not yet supported this compiler modify this macro to support"
#endif

/* 定义一些常见的空白字符，把他们当做空格处理 */
#define IS_SPACE_CHAR(x) ((x) == ' ' || (x) == '\t' || (x) == '\n' || (x) == '\r' || (x) == '\v' || (x) == '\f')
#define SKIP_SPACE(p) \
	while (IS_SPACE_CHAR(*(p))) ++(p);

static int decodeCMD(const char *p, miniCMD_cmd_t **cmd, miniCMD_AUTOdata_t *para);
static const char *decode_one_para(const char *str, miniCMD_AUTOdata_t *data);

static miniCMD_cmd_t *cmds; // 存储命令的段的基地址
static int cmd_num = 0;		// 命令数量（包括 help 命令）
void miniCMD_init(void)
{
	cmds = CMDS_start;
	cmd_num = CMDS_end - CMDS_start;
	CMD_print("\nminiCMD: %d commands init OK, input \"help()\" to show\n", cmd_num);
}

/* 打印所有 CMD 的命令 */
static int help(void)
{
	for (int i = 0; i < cmd_num; ++i)
		CMD_print("%d\t%s %s(%s)\n", i, cmds[i].fun_return_signature, cmds[i].fun_name, cmds[i].fun_para_signature);
	return cmd_num;
}
MAKE_CMD(int, help, void);

/**
 * @param str 要解析的字符串，必须确保以 '\0' 结尾，或者可以使用 miniCMD_run_s()。
 * @return 是命令运行成功
 * @author TanES (tan_163mail@163.com)
 */
int miniCMD_run(const char *str)
{
	/* 字符串解码 */
	miniCMD_cmd_t *cmd = 0;		 // 命令的解码结果
	miniCMD_AUTOdata_t para[25]; // 参数的解码结果
	int para_num = decodeCMD(str, &cmd, para);
	if (para_num < 0)
	{
		if (para_num == -1)
			CMD_print(">>%s :( NotFound command<<\n", str);
		else if (para_num == -2)
			CMD_print(">>%s :( format error<<\n", str);
		else
			CMD_print(">>%s :( parameter format error<<\n", str);
		return 0; // decode fail
	}

	/* print decode result */
	CMD_print(">>(");
	for (int i = 0; i < para_num; ++i)
	{
		if (para[i].type == type_f)
			CMD_print("%f ", para[i].var.F);
		else
			CMD_print("%d ", para[i].var.I);
	}
	CMD_print(")--->%s %s(%s)\n", cmd->fun_return_signature, cmd->fun_name, cmd->fun_para_signature);
	if (para_num != cmd->fun_para_num)
	{
		CMD_print("number of parameters: expect %d read %d<<\n", cmd->fun_para_num, para_num);
		return 0;
	}
	/* run and print return value */
	miniCMD_AUTOdata_t ret;
	cmd->fun_agent_ptr(&ret, para);
	if (ret.type == type_i)
		CMD_print("return %d<<\n", ret.var.I);
	else if (ret.type == type_f)
		CMD_print("return %f<<\n", ret.var.F);
	else
		CMD_print("return<<\n");
	return 1;
}
int miniCMD_run_s(char *str, unsigned int size)
{
	str[size - 1] = '\0';
	return miniCMD_run(str);
}

/**
 * @brief 字符串解码
 * @param p 字符串
 * @param cmd 解码结果：命令结构体指针
 * @param para 解码结果：参数列表
 * @return 参数数量，-1:未找到命令，-2:字符串格式错误，-3:参数解码失败
 */
static int decodeCMD(const char *p, miniCMD_cmd_t **cmd, miniCMD_AUTOdata_t *para)
{
	SKIP_SPACE(p);
	/* 检查格式: xxx(...) */
	const char *lParen = p;
	while (*lParen != '\0' && !IS_SPACE_CHAR(*lParen) && *lParen != '(' && *lParen != ',' && *lParen != ')')
		lParen++;
	SKIP_SPACE(lParen);
	if (*lParen != '(' || lParen == p) return -2; // 未找到 '('
	const char *rParen = lParen + 1;
	while (*rParen != '\0' && *rParen != '(' && *rParen != ')') rParen++;
	if (*rParen != ')') return -2; // "( )" 格式错误
	// ')'后只允许有';'和空白字符
	for (int i = 1; rParen[i] != '\0'; ++i)
	{
		if (rParen[i] != ';' && !IS_SPACE_CHAR(rParen[i]))
			return -2;
	}

	/* 匹配函数名 */
	*cmd = 0;
	for (int i = 0; i < cmd_num; ++i)
	{
		int j = 0;
		while (cmds[i].fun_name[j] != '\0' && cmds[i].fun_name[j] == p[j]) ++j;
		if (cmds[i].fun_name[j] == '\0' && (p[j] == '(' || IS_SPACE_CHAR(p[j])))
		{
			*cmd = &(cmds[i]);
			break;
		}
	}
	if (*cmd == 0) return -1; // 未找到命令
	/* 参数解码 */
	p = lParen + 1;
	SKIP_SPACE(p);
	if (*p == ')') return 0; // 没有参数
	for (int para_i = 0; *p != ',' && *p != ')';)
	{
		p = decode_one_para(p, &(para[para_i++]));
		if (p == 0) return -3;
		SKIP_SPACE(p);
		if (*p == ')') return para_i; // 解码结束
		if (*(p++) != ',') return -3; // 参数后只允许有 ")" 和 ","
		SKIP_SPACE(p);
	}
	return -2; // expect a parameter but read empty
}

/**
 * @brief 解码一个参数
 * @param str 要解码的字符串
 * @param data 解码结果
 * @return 下一个参数的指针，失败返回 0
 */
static const char *decode_one_para(const char *str, miniCMD_AUTOdata_t *data)
{
	data->type = type_i;
	data->var.I = 0; // 默认 {type_i,0}
	const char *p = str;
	/* 符号 */
	int sign = 0;
	if (*p == '-')
	{
		sign = 1;
		++p;
	}
	/* 整数部分 */
	if (*p < '0' || *p > '9') return 0; // 解码失败
	for (; *p >= '0' && *p <= '9'; ++p) data->var.I = data->var.I * 10 + (*p - '0');
	/* 小数部分 */
	if (*p == '.')
	{
		++p;
		data->type = type_f;
		/* 获取小数部分 */
		if (*p < '0' || *p > '9') return 0;
		int numF = 0, divisor = 1;
		for (; *p >= '0' && *p <= '9'; ++p)
		{
			numF = numF * 10 + (*p - '0');
			divisor *= 10;
		}
		data->var.F = (float)data->var.I + ((float)numF / (float)divisor);
	}
	/* 符号处理 */
	if (data->type == type_f && sign == 1)
		data->var.F = -data->var.F;
	else if (data->type == type_i && sign == 1)
		data->var.I = -data->var.I;
	return p;
}
