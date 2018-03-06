
#ifndef TESTAURIGA_TASK_H
#define TESTAURIGA_TASK_H

#include <future>
#include <memory>
#include <queue>

class CExcutor
{
public:
    virtual ~ CExcutor() = default;
    virtual void Excute(const std::function<void()> & functor) = 0;
};

class CWorkerThread
{
public:
    CWorkerThread()
            : m_bDone(false)
    {
        m_Thread = std::thread(&CWorkerThread::Start, this);
    }
    ~CWorkerThread()
    {
        Shutdown();
    }
    void Push(const std::function<void()> & rFunctor)
    {
        {
            std::lock_guard<std::mutex> lock(m_Mutex);
            m_quFunctors.push(rFunctor);
        }
        m_CondVar.notify_one();
    }
    void Shutdown()
    {
        {
            std::lock_guard<std::mutex> lock(m_Mutex);
            m_bDone = true;
        }
        m_CondVar.notify_one();
        m_Thread.join();
    }
private:
    void Start()
    {
        for(;;)
        {
            std::function<void()> functor;
            {
                std::unique_lock<std::mutex> lock(m_Mutex);


                m_CondVar.wait(lock, [this]{
                    return m_bDone || !m_quFunctors.empty();
                });
                if (m_bDone && m_quFunctors.empty()) {
                    break;
                }
                functor = m_quFunctors.front();
                m_quFunctors.pop();

            }
            functor();
        }
    }
    bool m_bDone;
    std::condition_variable m_CondVar;
    std::mutex m_Mutex;
    std::thread m_Thread;
    std::queue<std::function<void()>> m_quFunctors;
};

class CParallel : public CExcutor
{
public:
    void Excute(const std::function<void()> & functor) override
    {
        m_cWorkerThread.Push(functor);
    }

private:
    CWorkerThread m_cWorkerThread;
};

class CSequential: public CExcutor
{
public:
    void Excute(const std::function<void()> & functor) override
    {
        functor();
    }
};

template <typename Functor>
class CTask : public std::enable_shared_from_this<CTask<Functor>>
{
public:
    virtual ~CTask() = default;

    // delete copy/move semantics
    CTask(const CTask&) = delete;
    CTask& operator=(const CTask&) = delete;
    CTask(CTask&&) = delete;
    CTask& operator=(CTask&&) = delete;
    using result_type = decltype(std::declval<Functor>()());
    template <typename F>
    explicit CTask(F && functor)
    : m_Functor(std::forward<F>(functor))
    {

    }
    const std::shared_future<std::string> GetFuture() const noexcept
    {
        return m_Future;
    }
    void Schedule( CExcutor & excutor)
    {
        std::weak_ptr<CTask> same=this->shared_from_this();
        auto pack_task = std::make_shared<std::packaged_task<result_type()>>
                (std::bind(&CTask::RunTask, std::move(same)));
        m_Future = pack_task->get_future();

        excutor.Excute([pack_task](){(*pack_task)();});
    }
    void Wait() const
    {
        m_Future.wait();
    }
    static result_type RunTask(const std::weak_ptr<CTask> & task)
    {
        auto t = task.lock();
        if(!t)
        {
            std::cout << "task is expired"<<std::endl;
        }
        return t->m_Functor();
    }
public:
    Functor m_Functor;
    std::shared_future<std::string> m_Future;
    std::shared_ptr<CExcutor> m_Excutor;
};

template <typename Functor>
auto MakeTask(Functor && functor) -> decltype(std::make_shared<CTask<typename std::decay<Functor>::type>>(std::forward<Functor>(functor)))
{
    return std::make_shared<CTask<typename std::decay<Functor>::type>>(std::forward<Functor>(functor));
}

#endif //TESTAURIGA_TASK_H
