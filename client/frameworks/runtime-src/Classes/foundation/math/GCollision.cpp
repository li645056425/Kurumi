#include "GCollision.h"

#ifndef MIN
#define MIN(x,y) (((x) > (y)) ? (y) : (x))
#endif  // MIN

#ifndef MAX
#define MAX(x,y) (((x) < (y)) ? (y) : (x))
#endif  // MAX

bool GameMath_Tmp(const GVec2& IS, const GVec2* A, const GVec2* B)
{
	float v[4];
	for (int i = 0; i < 4; i++)
	{
		float tmp = (IS.x * A[i].x + IS.y * A[i].y) / (IS.x * IS.x + IS.y * IS.y);
		v[i] = tmp * IS.x * IS.x + tmp * IS.y * IS.y;
	}
	float vv[4];
	for (int i = 0; i < 4; i++)
	{
		float tmp = (IS.x * B[i].x + IS.y * B[i].y) / (IS.x * IS.x + IS.y * IS.y);
		vv[i] = tmp * IS.x * IS.x + tmp * IS.y * IS.y;
	}
	//��ʾ��ʱ��֪���Ƿ���ײ
	if (MAX(MAX(v[0], v[1]), MAX(v[2], v[3])) > MIN(MIN(vv[0], vv[1]), MIN(vv[2], vv[3])) && MIN(MIN(v[0], v[1]), MIN(v[2], v[3])) < MAX(MAX(vv[0], vv[1]), MAX(vv[2], vv[3])))
	{
		return false;
	}
	//��ʾ֪��δ��ײ
	else
	{
		return true;
	}
}

/// ���2�������Ƿ��ཻ
bool GCollision::isRectIntersect(const GVec2* A, const GVec2* B)
{
	static GVec2 Ax, Ay, Bx, By;

	Ax.x = A[0].x - A[1].x;
	Ax.y = A[0].y - A[1].y;
	Ay.x = A[0].x - A[3].x;
	Ay.y = A[0].y - A[3].y;
	Bx.x = B[0].x - B[1].x;
	Bx.y = B[0].y - B[1].y;
	By.x = B[0].x - B[3].x;
	By.y = B[0].y - B[3].y;

	if (GameMath_Tmp(Ax, A, B)) return false;
	if (GameMath_Tmp(Ay, A, B)) return false;
	if (GameMath_Tmp(Bx, A, B)) return false;
	if (GameMath_Tmp(By, A, B)) return false;
	return true;
}
