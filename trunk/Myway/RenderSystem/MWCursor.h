#pragma once

#include "MWCore.h"

namespace Myway {

	class MW_ENTRY Cursor
	{
		DECLARE_SINGLETON(Cursor);
		DECLARE_ALLOC();

	public:
		enum eSystemCursor {
			kArrow  = 32512,		// ��ͷ
			kIbeam  = 32513,		// ������ edit
			kWait = 32514,			// �ȴ�ɳ©
			kCross = 32515,			// ʮ��   ��
			kUpArrow = 32516,		// ���ϼ�ͷ ��
			kSize =  32646,			// ʮ�������ͷ
			kSizeNWSE = 32642,		// �������϶�˫���ͷ
			kSizeNESW = 32643,		// ����������˫���ͷ
			kSizeWE = 32644,		// ����˫���ͷ
			kSizeNS = 32645,		// �ϱ�˫���ͷ
			kNo = 32648,			// Բ��б�ܣ�no����
			kAppStarting = 32650,	// ARROW+WAIT
			kHelp = 32651,			// ARROW+�ʺ�
			kHand = 32649,			// ����
		};

	public:
		Cursor();
		~Cursor();

		void Load(int _dwResId);
		void Bind(int _dwResId);
		void Unbind();

		void Draw();

	protected:
		Map<int, HCURSOR> mCursorMap;
		HCURSOR mCursor;
		HCURSOR mLastCursor;
	};

}