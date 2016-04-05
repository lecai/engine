#pragma once


namespace PPE
{
	// ����Ĵ���ʹ���˴�����ģ�壬��Ϊ�˷���дЭ���Լ��ص�Э��
	// ����δ�����С��ά���ɱ���
	enum ECallResult
	{
		eCR_Success,			// ���ݳ������, call�ɹ�
		eCR_Again,				// ���ݳ��Ȳ�������Ҫ�����հ�
	};
	
	// TMsgHandler ���ڶ�Ӧ��Э���call back
	template<typename ImpClass>
	class TBaseMsgHandler
	{
	public:
		virtual const char* GetCmdName() = 0;
		virtual ECallResult Call(ImpClass* pDispatcher,void* pData,uint32 uSize,uint32& uCmdSize, void* pContext)=0;
		virtual void Release()=0;
	};


	template<typename InsClass,typename MsgClass>
	class TMsgHandler : public TBaseMsgHandler<InsClass>
	{
	public:
		typedef void (InsClass::*HandlerFun_t)(MsgClass* pData, void* pContext);

		TMsgHandler(HandlerFun_t pFun)
			:m_pHandlerFun(pFun)
		{
		}
		
		void Release()
		{
			PPE_DELETE_T( this, TMsgHandler, MEMCATEGORY_GENERAL);
		}

		const char* GetCmdName()
		{
			return MsgClass::GetName();
		}

		ECallResult Call(InsClass* pDispatcher,void* pData,uint32 uSize,uint32& uCmdSize, void* pContext)
		{
			uCmdSize = 0;
			if(uSize<MsgClass::GetHeaderSize())
				return eCR_Again;

			MsgClass* pMsg=static_cast<MsgClass*>(pData);

			uCmdSize=pMsg->GetPayloadSize()+MsgClass::GetHeaderSize();
			if(uCmdSize>uSize)
			{
				uCmdSize = 0;
				return eCR_Again;
			}

			( pDispatcher->*GetHandlerFun() )(pMsg, pContext);
			return eCR_Success;
		}

	protected:

		HandlerFun_t GetHandlerFun()const
		{
			return m_pHandlerFun;
		}
	private:
		HandlerFun_t m_pHandlerFun;
	};


	// TMsgHandlerContainer ����TMsgHandler������
	// ���� ID ��ӳ�� TMsgHandler
	// �˶����� TMsgDispatcher �ĳ�Ա
	template<typename InsClass>
	class TMsgHandlerContainer
	{
	public:
		typedef TBaseMsgHandler< InsClass>	BaseHandler_t;

		TMsgHandlerContainer(){};
		~TMsgHandlerContainer()
		{
			for(size_t i=0;i<m_vecHandler.size();++i)
			{
				m_vecHandler[i]->Release();
				m_vecHandler[i] = NULL;
			}
		}

		uint32 GetSize()
		{
			return (uint32)m_vecHandler.size();
		}

		bool Resize(uint32 uSize)
		{
			if(m_vecHandler.size()>uSize)
				return false;

			m_vecHandler.resize(uSize);

			return true;
		}

		BaseHandler_t* GetHandler( uint32 uIndex )
		{
			return m_vecHandler[uIndex];
		}

		void CheckHandlers()
		{
			for(size_t i=0;i<m_vecHandler.size();++i)
			{
				PPE_ASSERT(m_vecHandler[i]  != NULL);
			}
		}

		template<typename MsgClass>
		void SetHandler( void (InsClass::*HandlerFun) (MsgClass*, void*) )
		{
			typedef TMsgHandler<InsClass,MsgClass> Handler_t;
			Handler_t* pNewHandler = PPE_NEW_T( Handler_t, MEMCATEGORY_GENERAL, "")( HandlerFun );
			BaseHandler_t*& pHandler=m_vecHandler[ MsgClass::GetId() ];
			PPE_DELETE_T( pHandler, BaseHandler_t, MEMCATEGORY_GENERAL);
			pHandler=pNewHandler;
		}

	private:
		typename vector< BaseHandler_t* >::type	m_vecHandler;
	};

	// InsClass ��ʵ����������
	// IdType ��Э���ͷ��С: uint16 = 65536��Э�飬 uint8 = 256��Э��
	// TMsgDispatcher �����������Ķ����ṩLoopDispatch����OnceDispatch�Ĺ���
	// ���е�Э��ͨ�� SetHandler�ӿ� ע���TMsgDispatcher
	template<typename InsClass,typename IdType>
	class TMsgDispatcher
	{
	public:

		typedef TBaseMsgHandler<InsClass> BaseHandler_t;

		TMsgDispatcher()
			:m_bCanceled(false)
			,m_uDelayTime(0)
		{
		}

		virtual ~TMsgDispatcher()
		{

		}

		void CancelDispatch()
		{
			m_bCanceled=true;
		}

		void ResumeDispatch()
		{
			m_bCanceled=false;
		}

		bool DispatchIsCanceled()const
		{
			return m_bCanceled;
		}

		uint32 GetHandlerNumber()
		{
			return GetHandlers().GetSize();
		}

		bool SetHandlerNumber(uint32 uSize)
		{
			return GetHandlers().Resize(uSize);
		}

		BaseHandler_t* GetHandler( uint32 uIndex )
		{
			return GetHandlers().GetHandler(uIndex);
		}

		template<typename MsgClass>
		void SetHandler(  void (InsClass::*HandlerFun) (MsgClass*, void*) )
		{
			GetHandlers().SetHandler(HandlerFun);
		}

		// Ȼ���ٴ���ĳ�����ӵ����ݵ�ʱ�����֧�ִ�����ĳ��Э���
		// ֹͣ��������Э��
		bool LoopDispatch(const void* pData,uint32 uSize,uint32& uProcessedSize, void* pContext)
		{
			uint32 uTotalSize = uSize;
			uint32 uProcessedOnce=0;
			uProcessedSize = 0;
			char* pBuf = static_cast<char*>( const_cast<void*>(pData));
			for(;;)
			{
				bool bRes = OnceDispatch(pBuf, uTotalSize, uProcessedOnce, pContext);
				if(bRes)
				{
					pBuf+=uProcessedOnce;
					uTotalSize -= uProcessedOnce;
					uProcessedSize += uProcessedOnce;
				}
				else
				{
					if( uSize == uProcessedSize)
					{
						return true;
					}else
					{
						return false;
					}
				}
			}
		}

		// 
		bool LimitTimeLoopDispatch(uint32 uOnceTime, const void* pData,uint32 uSize,uint32& uProcessedSize, void* pContext)
		{
			uint32 uTotalSize = uSize;
			uint32 uProcessedOnce=0;
			uProcessedSize = 0;
			char* pBuf = static_cast<char*>( const_cast<void*>(pData));
			const uint32 uDelayFrame = 50;

			//std::cout << "LimitTimeLoopDispatch " << std::endl;
			bool bDoNext = true;
			while(bDoNext)
			{
				uint32 uBeginTime = CTimer::GetSingleton().getRealTime();
				bool bRes = OnceDispatch(pBuf, uTotalSize, uProcessedOnce, pContext);
				uint32 uEndTime = CTimer::GetSingleton().getRealTime();

				uint32 uTimeCost = uEndTime - uBeginTime;

				if(bRes)
				{
					pBuf+=uProcessedOnce;
					uTotalSize -= uProcessedOnce;
					uProcessedSize += uProcessedOnce;

					if( m_uDelayTime < uDelayFrame )
					{
						//std::cout << "m_uDelayTime < 5 " << std::endl;
						if(uTimeCost >= uOnceTime)
						{
							bDoNext = false;
							++m_uDelayTime;
							//std::cout << "�ӳٴ���������Ϣ" << std::endl;
							return true;
						}else
						{
							bDoNext = true;
							//std::cout << "uTimeCost < uOnceTime" << std::endl;
						}
					}else
					{
						//std::cout << "ǿ��ˢ��������Ϣ" << std::endl;
						bDoNext = true;
					}

				}
				else
				{
					if( uSize == uProcessedSize)
					{
						if(m_uDelayTime >= uDelayFrame)
						{
							//std::cout << "�ָ������ӳ�ˢ��" << std::endl;
							m_uDelayTime = 0;
						}
						//std::cout << "�������" << std::endl;
						return true;
					}else
					{
						//std::cout << "��δ����" << std::endl;
						return false;
					}
				}
			}
			return false;
		}

		void CheckHandlers()
		{
			ls_Handlers.CheckHandlers();
		}


		bool OnceDispatch(const void* pData,uint32 uSize,uint32& uProcessedSize, void* pContext)
		{
			if(DispatchIsCanceled())
				return false;

			if(uSize<sizeof(IdType))
				return false;

			char* pPos=static_cast<char*>( const_cast<void*>(pData));
			const IdType* pi = reinterpret_cast<const IdType*>(pPos);

			if( *pi >= GetHandlers().GetSize() )
				return false;

			TBaseMsgHandler<InsClass>* pHandler=GetHandlers().GetHandler(*pi);
			if( !pHandler )
				return false;

			ECallResult eResult=pHandler->Call(static_cast<InsClass*>(this),pPos, uSize, uProcessedSize, pContext);
			switch(eResult)
			{	
			case eCR_Success:
				return true;

			case eCR_Again:
				return false;
			default:
				PPE_ASSERT(false);
			}

			return false;
		}

	private:
		TMsgHandlerContainer<InsClass>& GetHandlers()
		{
			return ls_Handlers;
		}

	private:
		bool														m_bCanceled; // �Ƿ����dispatch�ı�־
		TMsgHandlerContainer<InsClass>		ls_Handlers;	  //  ����handler�Ĺ�����
		uint32														m_uDelayTime;
	};



	///  Э�����
	///  ����Э�鲻��Ҫʵ��GetPayloadSizeDerived����
	///  �䳤Э����Ҫ����GetPayloadSizeDerived������return��ȷ�����ݳ���ֵ
#pragma pack(push,1)
	template<typename InsClass,typename IdType,IdType uId>
	class TBasePrtlMsg
	{
	private:
		uint32 GetPayloadSizeDerived()
		{
			return 0;
		}
	public:
		TBasePrtlMsg() :m_uId(uId)
		{
		}

		static uint32 GetHeaderSize()
		{
			return sizeof(InsClass);
		}
		static IdType GetId()
		{
			return uId;
		}
		uint32 GetPayloadSize()
		{
			return static_cast<InsClass*>(this)->GetPayloadSizeDerived();
		}
	private:
		IdType	m_uId;
	};
#pragma pack(pop)
}
