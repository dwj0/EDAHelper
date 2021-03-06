#ifndef	__PROTEL_H
#define	__PROTEL_H

enum	ProtelEnable_e {
	PROTEL_GLOBAL_ENABLE	= 1,
	PROTEL_MIDBTN_ZOOM		= 1 << 1,
	PROTEL_RIGBTN_DRAG		= 1 << 2,
	PROTEL_MIDBTN_SWITCH	= 1 << 3,
	PROTEL_MIDBTN_PLACE		= 1 << 4,
	PROTEL_MIDBTN_MOVE		= 1 << 5,
	PROTEL_RIGBTN_ROTATE	= 1 << 6,
	PROTEL_KEY_SWITCH			= 1<< 7
};

#define	AD_C_SWITCH

BOOL	ProtelInit(void);
UINT	ProtelGetConf(void);
void	ProtelSetConf(UINT value);
#endif

