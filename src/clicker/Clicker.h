#pragma once

//left clicker
class c_clicker {
protected:
    //vars
    int type2counterboost = 0;

    int type2counterboost2 = 0;
    bool type2boost = false;
    bool type2boost2 = false;
    int type2counter = 0;
    int type2counter2 = 0;
    bool type2drop2 = false;

    bool first_click = false;

    bool inconsistensy = false;
    bool inconsistensy2 = false;
    int inconsistensycounter = 0;
    int inconsistensycounter2 = 0;

    float min = 0.4f;
    float max = 0.8f;
    float min2 = 0.4f;
    float max2 = 0.8f;
    bool fonce = false;
    int spikemaxchance = 10;
    int chancespike = 0;

    float mincanboostchance = 12.f;
    float mincandropchance = 12.f;

    bool can_stop = true;

    void reset_vars();
public:
   void thread();
};

inline auto p_clicker = std::make_unique<c_clicker>();

//right clicker
class c_right_clicker : private c_clicker {
public:
    void thread();
};

inline auto p_right_clicker = std::make_unique<c_right_clicker>();
