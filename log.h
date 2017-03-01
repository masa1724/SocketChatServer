#ifndef LIB_LOG_H_
#define LIB_LOG_H_

#include <stdio.h>
#include <time.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <sys/time.h>
#include <stdarg.h>
#include <tchar.h>
#include <windows.h>
#include <stdarg.h>


/*
 *  ���O�t�@�C���փ��b�Z�[�W���o�͂���.
 *  �����L�֐��͒��ڌĂяo�����A��L�̃}�N������ČĂяo���ĉ������B
 *
 *  @param *fileName : �t�@�C����
 *  @param *funcName : �֐���
 *  @param lineNum   : �s�ԍ�
 *  @param *format   : ������������
 *  @param ...       : �ϒ��p�����[�^
 *
 */
void _infolog(LPCTSTR fileName, LPCTSTR funcName, int lineNum, LPCTSTR format, ...);
void _warnlog(LPCTSTR fileName, LPCTSTR funcName, int lineNum, LPCTSTR format, ...);
void _errorlog(LPCTSTR fileName, LPCTSTR funcName, int lineNum, LPCTSTR format, ...);


#define warnlog(format, ...) _warnlog((LPTSTR)__FILE__, (LPTSTR)__FUNCTION__, __LINE__, format, ##__VA_ARGS__)
#define errorlog(format, ...) _errorlog((LPTSTR)__FILE__, (LPTSTR)__FUNCTION__, __LINE__, format, ##__VA_ARGS__)
#define infolog(format, ...) _infolog((LPTSTR)__FILE__, (LPTSTR)__FUNCTION__, __LINE__, format, ##__VA_ARGS__)

#endif /* LIB_LOG_H_ */
