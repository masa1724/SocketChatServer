#include "log.h"

// アプリログファイル
#define LOG_DIR_PATH TEXT("C:\\var\\log\\clang\\")

// アプリログファイル
#define APL_LOG_NAME TEXT("server.log")

// エラーログファイル
#define ERROR_LOG_NAME TEXT("server_error.log")

// アプリログファイル
#define LOG_BACKUP_DIR_PATH TEXT("C:\\var\\log\\clang\\backup\\")

// ログサイズの最大サイズ 2M (byte単位)
#define LOG_FILE_SIZE_MAX 20000000


/* ログ出力情報 */
typedef struct __LOG_INFO_T__ {
	TCHAR title[5];      // タイトル
	TCHAR filePath[200]; // ファイルパス
} LOG_INFO_T;


void writelog(LPCTSTR msg, LOG_INFO_T *info, LPCTSTR fileName, LPCTSTR funcName, int lineNum);
int iscyclelog(LPCTSTR filePath);
int backuplog(LPCTSTR filePath);


#ifdef UNICODE
#define __stprintf(dist, format, ...) _stprintf(dist, _tcslen(dist)+2, format, ##__VA_ARGS__)
#else
#define __stprintf(dist, format, ...) _stprintf(dist, format, ##__VA_ARGS__)
#endif

#ifdef UNICODE
#define __vstprintf(dist, format, ...) _vstprintf(msg, _tcslen(dist)+2, format, ##__VA_ARGS__);
#else
#define __vstprintf(dist, format, ...) _vstprintf(dist, format, ##__VA_ARGS__);
#endif


void
_infolog(LPCTSTR fileName, LPCTSTR funcName, int lineNum, LPCTSTR format, ...) {
	LOG_INFO_T info;
	_tcscpy(info.title, TEXT("Info"));
	__stprintf(info.filePath, TEXT("%s%s"), LOG_DIR_PATH, APL_LOG_NAME);

	TCHAR msg[1024];
	va_list args;
	va_start(args, format);
	__vstprintf(msg, format, args);
	writelog(msg, &info, fileName, funcName, lineNum);
	va_end(args);
}

void
_warnlog(LPCTSTR fileName, LPCTSTR funcName, int lineNum, LPCTSTR format, ...) {
	LOG_INFO_T info;
	_tcscpy(info.title, TEXT("Warn"));
	__stprintf(info.filePath, TEXT("%s%s"), LOG_DIR_PATH, ERROR_LOG_NAME);

	TCHAR msg[1024];
	va_list args;
	va_start(args, format);
	__vstprintf(msg, format, args);
	writelog(msg, &info, fileName, funcName, lineNum);
	va_end(args);
}

void
_errorlog(LPCTSTR fileName, LPCTSTR funcName, int lineNum, LPCTSTR format, ...) {
	LOG_INFO_T info;
	_tcscpy(info.title, TEXT("Err "));
	__stprintf(info.filePath, TEXT("%s%s"), LOG_DIR_PATH, ERROR_LOG_NAME);

	TCHAR msg[1024];
	va_list args;
	va_start(args, format);
	__vstprintf(msg, format, args);
	writelog(msg, &info, fileName, funcName, lineNum);
	va_end(args);
}

void
writelog(
		LPCTSTR msg,
		LOG_INFO_T *info,
		LPCTSTR fileName,
		LPCTSTR funcName,
		int lineNum) {

	struct timeval tv;
	time_t t;
	struct tm *ts;
	gettimeofday(&tv, NULL);
	t = (time_t)tv.tv_sec;
	ts = localtime(&t);

	FILE *fp;
	fp = _tfopen(info->filePath, TEXT("a"));
	if(fp == NULL) {
		_tprintf(TEXT("aaaaaaaaaaaaaaaaaaaaaa\n"));
	}

	// ログファイルへメッセージ出力
	_ftprintf(fp,
			TEXT("[%s] %04d-%02d-%02d.%02d:%02d:%02d.%06ld pgname:[%s] funcName:[%s] lineNum:[%d] %s\n"),
			info->title,
			ts->tm_year+1900, ts->tm_mon+1, ts->tm_mday, ts->tm_hour, ts->tm_min, ts->tm_sec, tv.tv_usec,
			fileName, funcName, lineNum, msg);

	fclose(fp);

	// ログファイルの周期させるか判定
	if (iscyclelog(info->filePath) == 0) {
		backuplog(info->filePath);
	}
}

int
iscyclelog(LPCTSTR filePath) {
	struct _stat st;

	int ret = _tstat(filePath, &st);

	// ファイル存在しない場合
	if(ret != 0) {
		return 0;
	}

	int size = st.st_size;

	// ファイルサイズが最大値超え
	if(size >= LOG_FILE_SIZE_MAX) {
		return 0;
	}

	return -1;
}

int
backuplog(LPCTSTR filePath) {
	TCHAR *fileName;
	time_t timer;
	TCHAR newFilePath[200];

	// ファイル名を取得
	fileName = _tcsrchr(filePath, TEXT('\\')) + 1;

	// 現在日時を取得
	time(&timer);
	struct tm *ts = localtime(&timer);


	// ログファイルパスを生成
	__stprintf(newFilePath,
			 TEXT("%s%04d%02d%02d%02d%02d%02d%s"),
					LOG_BACKUP_DIR_PATH,
					ts->tm_year+1900, ts->tm_mon+1, ts->tm_mday, ts->tm_hour, ts->tm_min, ts->tm_sec,
					fileName);


	// backupフォルダに移動
	_trename(filePath, newFilePath);

	return 0;
}
