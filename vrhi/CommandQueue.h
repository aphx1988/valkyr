namespace valkyr{
    enum CommandListType{
        Direct,
        Bundle,
        Compute,
        Copy
    };

    enum CommandQueuePriority{
        Normal,
        High,
        GlobalRealtime
    };

    enum CommandQueueFlag{
        None,
        DisableGpuTimeout
    };

    class CommandQueue{
    public:
        CommandListType m_type;
        CommandQueuePriority m_priority;
        CommandQueueFlag m_flag;
        unsigned int m_nodeMask;
    };

    class CommandList{
    public:
        virtual void ClearRTV()=0;
        virtual void ClearDSV()=0;
    };

}