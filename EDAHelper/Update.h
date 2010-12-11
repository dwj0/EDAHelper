#ifndef __UPDATE_H
#define	CURRENT_VERSION "2.0.11"
#define	UPDATE_CHECK_PERIOD (1000*60*60*24)

#define	NEW_VERSION_TIP		"发现新版本："
#define	NONEW_VERSION_TIP	"您的版本已是最新"

void UpdateCheck(bool IsQuiet);
DWORD WINAPI UpdateTask( LPVOID lpParameter );

#endif