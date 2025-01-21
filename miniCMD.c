/** miniCMD V1.0
 * @file miniCMD.c
 * @author TanES (tan_163mail@163.com)
 */
#include "miniCMD.h"
#include "miniCMD_makeCmd.h"
#include "miniCMD_config.h"

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

#define IS_SPACE_CHAR(x) ((x) == ' ' || (x) == '\t' || (x) == '\n' || (x) == '\r' || (x) == '\v' || (x) == '\f')
#define SKIP_SPACE(p)           \
	while (IS_SPACE_CHAR(*(p))) \
	++(p)
/* decode string to function and parameters */
static int decodeCMD(const char *p, miniCMD_cmd_t **cmd, miniCMD_AUTOdata_t *para);
/* Converting a string to a number (only be called by decodeCMD) */
static const char *decode_one_para(const char *str, miniCMD_AUTOdata_t *data);

static miniCMD_cmd_t *cmds; // base address of commands
static int cmd_num = 0;		// number of commands (include helpCommand)
void miniCMD_init(void)
{
	cmds = CMDS_start;
	cmd_num = CMDS_end - CMDS_start;
	CMD_print("\nminiCMD: %d commands init OK, input \"help()\" to show\n", cmd_num);
}

/* The default cmd to print all CMD */
static int help(void)
{
	for (int i = 0; i < cmd_num; ++i)
		CMD_print("%d\t%s %s(%s)\n", i, cmds[i].fun_return_signature, cmds[i].fun_name, cmds[i].fun_para_signature);
	return cmd_num;
}
MAKE_CMD(int, help, void);

/**
 * @param str must ensure ending with '\0', or can use miniCMD_run_s()
 * @return is command run success
 * @author TanES (tan_163mail@163.com)
 */
int miniCMD_run(const char *str)
{
	/* decode string */
	miniCMD_cmd_t *cmd = 0;		 // Point to decoding result
	miniCMD_AUTOdata_t para[25]; // storage input parameters
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
 * @return number of parameter
 * -1:not found command 
 * -2:format error 
 * -3:parameter decode fail
 */
static int decodeCMD(const char *p, miniCMD_cmd_t **cmd, miniCMD_AUTOdata_t *para)
{
	SKIP_SPACE(p);
	/* check format: xxx(...) */
	const char *lParen = p;
	while (*lParen != '\0' && !IS_SPACE_CHAR(*lParen) && *lParen != '(' && *lParen != ',' && *lParen != ')')
		lParen++;
	SKIP_SPACE(lParen);
	if (*lParen != '(' || lParen == p)
		return -2; // not found '(' or no function name
	const char *rParen = lParen + 1;
	while (*rParen != '\0' && *rParen != '(' && *rParen != ')')
		rParen++;
	if (*rParen != ')')
		return -2; // format error:  "( )"
	// Only ';' and space are allowed after ')'
	for (int i = 1; rParen[i] != '\0'; ++i)
	{
		if (rParen[i] != ';' && !IS_SPACE_CHAR(rParen[i]))
			return -2;
	}

	/* Match function name */
	*cmd = 0;
	for (int i = 0; i < cmd_num; ++i)
	{
		int j = 0;
		while (cmds[i].fun_name[j] != '\0' && cmds[i].fun_name[j] == p[j])
			++j;
		if (cmds[i].fun_name[j] == '\0' && (p[j] == '(' || IS_SPACE_CHAR(p[j])))
		{
			*cmd = &(cmds[i]);
			break;
		}
	}
	if (*cmd == 0)
		return -1; // not found command

	/* Getting Parameters */
	p = lParen + 1;
	SKIP_SPACE(p);
	if (*p == ')')
		return 0;
	for (int para_i = 0; *p != ',' && *p != ')';)
	{
		p = decode_one_para(p, &(para[para_i++]));
		if (p == 0)
			return -3;
		SKIP_SPACE(p);
		if (*p == ')')
			return para_i;
		if (*(p++) != ',')
			return -3; // Only ')' and ',' are allowed after parameter
		SKIP_SPACE(p);
	}
	return -2; // expect a parameter but read empty
}

static const char *decode_one_para(const char *str, miniCMD_AUTOdata_t *data)
{
	data->type = type_i;
	data->var.I = 0; // data default to {type_i,0}
	const char *p = str;
	/* Get sign */
	int sign = 0;
	if (*p == '-')
	{
		sign = 1;
		++p;
	}
	/* Get the integer part */
	if (*p < '0' || *p > '9')
		return 0; // decode this parameter fail
	for (; *p >= '0' && *p <= '9'; ++p)
		data->var.I = data->var.I * 10 + (*p - '0');
	/* Process decimal float type */
	if (*p == '.')
	{
		++p;
		data->type = type_f;
		/* Getting the fractional part */
		if (*p < '0' || *p > '9')
			return 0;
		int numF = 0, divisor = 1;
		for (; *p >= '0' && *p <= '9'; ++p)
		{
			numF = numF * 10 + (*p - '0');
			divisor *= 10;
		}
		data->var.F = (float)data->var.I + ((float)numF / (float)divisor);
	}
	/* Process sign */
	if (data->type == type_f && sign == 1)
		data->var.F = -data->var.F;
	else if (data->type == type_i && sign == 1)
		data->var.I = -data->var.I;
	return p;
}
