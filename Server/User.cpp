#include "StdAfx.h"

#include "User.h"

User::User()
{
    mIStream = new NetIStream(&mSock);
    mOStream = new NetOStream(&mSock);

	mState = UNKNOWN;

	mUserId = INVALIDE_USER_ID;
	mUserPower = POWER_GM;

	mIp = "";
	mAccount = "";

	mPlayer = NULL;
}

User::~User()
{
    delete mIStream;
    delete mOStream;
}

void User::Init()
{
    mState = UNKNOWN;
}

void User::Shutdown()
{
    mState = UNKNOWN;

    Socket::Close(&mSock);

    mUserId = INVALIDE_USER_ID;
	mIp = "";
	mAccount = "";

	mPlayer = NULL;
}

void User::SetAccount(const char * number)
{
	mAccount = number;
}

Socket * User::GetSocket()
{
    return &mSock;
}

void User::SetState(int state)
{
    mState = state;

    if (state == CONNECTED)
    {
		mIp = Socket::GetIp(&mSock);
    }
}

int User::GetState()
{
    return mState;
}

int User::GetUserId()
{
    return mUserId;
}

const char * User::GetUserIp() const
{
    return mIp.c_str();
}

bool User::SendPacket(NetPacket * pack)
{
    if (!IsValid())
        return false;

	PkOStream stream;
	
	pack->Out(stream);

    char buffer[MAX_PACK_SIZE];

    NetPacketHead head;

    head.id = pack->GetPacketId();
    head.size = (short)stream.Size();
    head.tick = 0;

    int offset = 0;
    WriteBuffer(buffer, MAX_PACK_SIZE, 0, PACK_COMPACK, PACK_COMPACK_SIZE);
    offset += PACK_COMPACK_SIZE;
    WriteBuffer(buffer, MAX_PACK_SIZE, offset, &head, NetPacketHead::SIZE);
    offset += NetPacketHead::SIZE;

    WriteBuffer(buffer, MAX_PACK_SIZE, offset, stream.Data(), head.size);
    offset += head.size;

    // ������Ϣͷ
    NET_ENCRYPT(buffer + PACK_COMPACK_SIZE, NetPacketHead::SIZE, PACK_HEAD_KEY, PACK_HEAD_SPACER);
    // ������Ϣ��
    NET_ENCRYPT(buffer + PACK_COMPACK_SIZE + NetPacketHead::SIZE, head.size, PACK_BODY_KEY, PACK_BODY_SPACER);

    mOStream->Write(buffer, offset);

    return true;
}

bool User::_Input()
{
    int ret = mIStream->Refresh();

    if (ret <= NET_ERROR)
    {
        return false;
    }

    return true;
}

bool User::_Output()
{
    int ret = mOStream->Refresh();

    if (ret <= NET_ERROR)
    {
        return false;
    }

    return true;
}

bool User::_Process()
{
    char buffer[MAX_PACK_SIZE] = { 0 };

    while (1)
    {
        NetPacketHead head;

        int skip = 0;

        // �����ָ��
        if (!mIStream->Skip(PACK_COMPACK, PACK_COMPACK_SIZE))
        {  
            break;
        }

		skip += PACK_COMPACK_SIZE;

        // ��ȡ��Ϣͷ
        if (mIStream->Read(&head, NetPacketHead::SIZE) == 0)
        {
            // û�н�����ȫ
            mIStream->Skip(-skip);
            break;
        }

		skip += NetPacketHead::SIZE;

        // ����
        NET_ENCRYPT(&head, NetPacketHead::SIZE, PACK_HEAD_KEY, PACK_HEAD_SPACER);

        if (head.id >= NPI_MAX)
        {
            printf("Warning: Net Packet Id Error, %d\n", head.id);
            break;
        }

        if (head.size < 0 || head.size > MAX_PACK_SIZE - NetPacketHead::SIZE - PACK_COMPACK_SIZE)
        {
            printf("Warning: Net Pack Size Error, id %d, size %d\n", head.id, head.size);
            break;
        }

        // ��ȡ��Ϣ
        if (mIStream->Read(buffer, head.size) == 0)
        {
            // ��δ������ȫ
            mIStream->Skip(-skip);
            break;
        }

        // ����
        NET_ENCRYPT(buffer, head.size, PACK_BODY_KEY, PACK_BODY_SPACER);

        // �������ݰ�
        NetPacket * pack = NetPacketManager::Instance()->GetPacket(head.id);

        d_assert (pack && "���ݰ����������δע��.");

		pack->In(PkIStream(buffer, head.size));
		pack->Execute(this);
    }

    return true;
}

bool User::IsValid()
{
    return mUserId != INVALIDE_USER_ID && mUserId >= 0 && mUserId < MAX_USER && Socket::IsValid(&mSock);
}
