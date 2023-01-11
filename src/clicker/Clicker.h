#pragma once

//left clicker
class c_clicker {
protected:

    enum class mouse_type
    {
        LEFT,
        RIGHT
    };

    std::thread mthread;
    std::atomic_bool m_threadFlag;

    //vars
    int boost_counter = 0;

    int boost_counter2 = 0;
    bool boost = false;
    bool boost2 = false;
    int counter = 0;
    int counter2 = 0;
    bool bdrop = false;

    bool first_click = false;

    bool inconsistensy = false;
    bool inconsistensy2 = false;
    int inconsistensycounter = 0;
    int inconsistensycounter2 = 0;

    float min = 0.4f;
    float max = 0.8f;
    bool bonce = false;
    int spikemaxchance = 10;
    int chancespike = 0;

    float mincanboostchance = 12.f;
    float mincandropchance = 12.f;

    bool can_stop = true;

    void reset_vars();
    
    void send_down(mouse_type mb, POINT& pt, float& sometingdelay, float delayclick2);
    void send_up(mouse_type mb, POINT& pt, float& sometingdelay, float delayclick2);

    void thread();

public:
    void start_thread() 
    {
        m_threadFlag = true;
        mthread = std::thread(&c_clicker::thread, this);
    }
    void stop_thread()
    {
        m_threadFlag = false;
        if (mthread.joinable())  mthread.join();
    }

    bool is_thread_alive() const
    {
        return m_threadFlag;
    }
};

inline auto p_clicker = std::make_unique<c_clicker>();

//right clicker
class c_right_clicker : private c_clicker {
private:
    void thread_right();
public:
    void start_thread()
    {
        m_threadFlag = true;
        mthread = std::thread(&c_right_clicker::thread_right, this);
    }
    void stop_thread()
    {
        m_threadFlag = false;
        if (mthread.joinable())  mthread.join();
    }

    bool is_thread_alive() const
    {
        return m_threadFlag;
    }
};

inline auto p_right_clicker = std::make_unique<c_right_clicker>();