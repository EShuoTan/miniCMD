/** miniCMD V1.0
 * @file miniCMD_makeCmd.h UTF-8
 * 本文件提供 MAKE_CMD() 宏以添加命令到 miniCMD，可以被任意文件include，
 * 但是只建议在 miniCMD_config.h 中执行 MAKE_CMD()，
 * 因为本文件会引入很多没用的宏和一个代理函数
 * 
 * This file provides the MAKE_CMD() macro, 
 * which you can include in any file to add commands.
 * But it is recommended to add all commands in miniCMD_config.h
 * Because this file will introduce a lot of useless macros, and a proxy function
 */
#ifndef miniCMD_makeCmd_h
#define miniCMD_makeCmd_h

#if defined(__GNUC__) || defined(__CC_ARM) || (defined(__ARMCC_VERSION) && __ARMCC_VERSION >= 6000000)
#define USED __attribute__((used))
#define IN_CMDs_SECTION __attribute__((section("miniCMDcommands"), aligned(1)))
#else
#error "miniCMD not yet supported this compiler modify this macro to support"
#endif

typedef struct
{
	enum type_e
	{
		type_i,
		type_f,
		type_void
	} type;
	union IF_t
	{
		int I;
		float F;
	} var;
} miniCMD_AUTOdata_t;
typedef const struct
{
	char *fun_name;				// For matching functions
	char *fun_return_signature; // Type of function return value, in string form
	char *fun_para_signature;	// List of function parameter types, in string form
	unsigned char fun_para_num; // Number of function parameters
	void (*fun_agent_ptr)(miniCMD_AUTOdata_t *ret, miniCMD_AUTOdata_t *para);
} miniCMD_cmd_t;

#define xxx_miniCMD_AUTOgetData(x) (((x).type == type_f) ? ((x).var.F) : ((x).var.I))
#define xxx_miniCMD_getDatapara_X0
#define xxx_miniCMD_getDatapara_X1 xxx_miniCMD_AUTOgetData(para[0])
#define xxx_miniCMD_getDatapara_X2 xxx_miniCMD_getDatapara_X1, xxx_miniCMD_AUTOgetData(para[1])
#define xxx_miniCMD_getDatapara_X3 xxx_miniCMD_getDatapara_X2, xxx_miniCMD_AUTOgetData(para[2])
#define xxx_miniCMD_getDatapara_X4 xxx_miniCMD_getDatapara_X3, xxx_miniCMD_AUTOgetData(para[3])
#define xxx_miniCMD_getDatapara_X5 xxx_miniCMD_getDatapara_X4, xxx_miniCMD_AUTOgetData(para[4])
#define xxx_miniCMD_getDatapara_X6 xxx_miniCMD_getDatapara_X5, xxx_miniCMD_AUTOgetData(para[5])
#define xxx_miniCMD_getDatapara_X7 xxx_miniCMD_getDatapara_X6, xxx_miniCMD_AUTOgetData(para[6])
#define xxx_miniCMD_getDatapara_X8 xxx_miniCMD_getDatapara_X7, xxx_miniCMD_AUTOgetData(para[7])
#define xxx_miniCMD_getDatapara_X9 xxx_miniCMD_getDatapara_X8, xxx_miniCMD_AUTOgetData(para[8])
#define xxx_miniCMD_getDatapara_X10 xxx_miniCMD_getDatapara_X9, xxx_miniCMD_AUTOgetData(para[9])
#define xxx_miniCMD_getDatapara_X11 xxx_miniCMD_getDatapara_X10, xxx_miniCMD_AUTOgetData(para[10])
#define xxx_miniCMD_getDatapara_X12 xxx_miniCMD_getDatapara_X11, xxx_miniCMD_AUTOgetData(para[11])
#define xxx_miniCMD_getDatapara_X13 xxx_miniCMD_getDatapara_X12, xxx_miniCMD_AUTOgetData(para[12])
#define xxx_miniCMD_getDatapara_X14 xxx_miniCMD_getDatapara_X13, xxx_miniCMD_AUTOgetData(para[13])
#define xxx_miniCMD_getDatapara_X15 xxx_miniCMD_getDatapara_X14, xxx_miniCMD_AUTOgetData(para[14])
#define xxx_miniCMD_getDatapara_X16 xxx_miniCMD_getDatapara_X15, xxx_miniCMD_AUTOgetData(para[15])
#define xxx_miniCMD_getDatapara_X17 xxx_miniCMD_getDatapara_X16, xxx_miniCMD_AUTOgetData(para[16])
#define xxx_miniCMD_getDatapara_X18 xxx_miniCMD_getDatapara_X17, xxx_miniCMD_AUTOgetData(para[17])
#define xxx_miniCMD_getDatapara_X19 xxx_miniCMD_getDatapara_X18, xxx_miniCMD_AUTOgetData(para[18])
#define xxx_miniCMD_getDatapara_X20 xxx_miniCMD_getDatapara_X19, xxx_miniCMD_AUTOgetData(para[19])

#define GET_ARGS_1th(_1, ...) _1
#define GET_ARGS_4th(_1, _2, _3, _4, ...) _4
#define GET_ARGS_21th(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, ...) _21
#define GET_ARGS_nth(_n, ...) GET_ARGS_##_n##th(__VA_ARGS__)

/* MATCH: void float other*/
#define COMMA_void2_float1_void _, _, _
#define COMMA_void2_float1_float _, _
#define PRIMITIVE_vfi_TYPE_MATCH(x, _is_void, _is_float, _is_other) \
	GET_ARGS_nth(4, COMMA_void2_float1_##x, _is_void, _is_float, _is_other)
#define vfi_TYPE_MATCH(x, _is_void, _is_float, _is_other) PRIMITIVE_vfi_TYPE_MATCH(x, _is_void, _is_float, _is_other)

#define PRIMITIVE_GLUE(x, y) x##y
#define GLUE(x, y) PRIMITIVE_GLUE(x, y)

/* multi-parameter judgment empty */
#define CHECK_HAS_COMMA_max20(...) GET_ARGS_21th(__VA_ARGS__, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0)
#define COMMA01_01 _, _
#define IS_EMPTY(...)                               \
	CHECK_HAS_COMMA_max20(                          \
		GLUE(                                       \
			COMMA01_,                               \
			GLUE(                                   \
				CHECK_HAS_COMMA_max20(__VA_ARGS__), \
				CHECK_HAS_COMMA_max20(COMMA01_01##__VA_ARGS__))))

/* determine the number of parameters */
#define COUNT_ARGS_IS_EMPTY_1(...) 0
#define COUNT_ARGS_IS_EMPTY_0(...) \
	GET_ARGS_21th(__VA_ARGS__, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1)
#define COUNT_ARGS_max20(...) GLUE(COUNT_ARGS_IS_EMPTY_, IS_EMPTY(__VA_ARGS__))(__VA_ARGS__)

/* determine the number of parameters, if the first is `void` also return 0 */
#define COUNT_PARA(...) \
	vfi_TYPE_MATCH(GET_ARGS_1th(__VA_ARGS__), 0, COUNT_ARGS_max20(__VA_ARGS__), COUNT_ARGS_max20(__VA_ARGS__))

#define MAKE_CMD(ret_type, name, ...)                                                            \
	extern ret_type name(__VA_ARGS__);                                                           \
	static void x___##name##_miniCMD_agentFun(miniCMD_AUTOdata_t *ret, miniCMD_AUTOdata_t *para) \
	{                                                                                            \
		ret->type = vfi_TYPE_MATCH(ret_type, type_void, type_f, type_i);                         \
		vfi_TYPE_MATCH(ret_type, , ret->var.F =, ret->var.I =)                                   \
			name(GLUE(xxx_miniCMD_getDatapara_X, COUNT_PARA(__VA_ARGS__)));                      \
	}                                                                                            \
	USED IN_CMDs_SECTION const miniCMD_cmd_t x___miniCMD_cmd_##name =                                 \
		{.fun_name = #name,                                                                      \
		 .fun_agent_ptr = x___##name##_miniCMD_agentFun,                                         \
		 .fun_return_signature = #ret_type,                                                      \
		 .fun_para_signature = #__VA_ARGS__,                                                     \
		 .fun_para_num = COUNT_PARA(__VA_ARGS__)}

#endif
