class Manager {
	protected:
		bool started = false;
	public:
		virtual ~Manager() = default;
		virtual int startUp();
		virtual void shutDown();
		virtual bool isStarted() const { return started; }
};