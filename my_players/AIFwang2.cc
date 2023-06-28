
#include "Player.hh"

using namespace std;


/**
 * Write the name of your player and save this file
 * as AI<name>.cc
 */
#define PLAYER_NAME Fwang2





struct PLAYER_NAME : public Player {


    /**
     * Factory: returns a new instance of this class.
     * Do not modify this function.
     */
    static Player* factory () {
        return new PLAYER_NAME;
    }
    

    /**
     * Attributes for your player can be defined here.
     */     
    typedef vector<bool> VB;
    typedef vector<VB>  VVB;
    typedef vector<Pos> VP;
    typedef vector<VP> VVP;
    typedef vector<Dir> VD;

    VD all_dirs;

    bool can_move(const Pos& p, const Dir& d)
    {
        const auto it = dir2all.find(d);
        for(const Dir& dir : it->second) {
            CType t = cell(p+dir).type;
            if(t == TYRE or t == CAR or t == MISSILE) return false;
        }
        return true;
    }

    bool has_bonus(const Pos& p) {
        CType t = cell(p).type;
        return t == WATER_BONUS or t == MISSILE_BONUS or t == GAS_BONUS;
    }

    bool fast(const Dir& d) {
        return d == FAST or d == FAST_DOWN or d == FAST_UP; 
    }

    bool normal(const Dir& d) {
        return d == DEFAULT or d == DOWN or d == UP; 
    }

    bool slow(const Dir& d) {
        return d == SLOW_DOWN or d == SLOW_UP;
    }

    void BFS(Pos& p,const CType& elem)
    {
        int i = first(p);
        int j = second(p);
        int rows = number_rows();
        int cols = number_window_columns();
        VVB vis(rows, VB(cols, false));
        VVP previous(rows, VP(cols));
        queue<Pos> q;

        vis[i][j%cols] = 0;
        q.push(Pos(i,j));

        while (not q.empty()) {
            Pos curr = q.front();
            q.pop();

            int r = first(curr);
            int c = second(curr);

            for(Dir d: all_dirs) {
                Pos new_p = curr+d;
                int rr = first(new_p);
                int cc = second(new_p);
                if(within_window(new_p,round()+1) and can_move(curr,d) and not vis[rr][cc%cols]) {
                    q.push(new_p);
                    vis[rr][cc%cols] = true;
                    previous[rr][cc%cols] = curr;
                    CType t = cell(new_p).type;
                    if(t == elem) {
                        bool first_step = false;
                        while (not first_step) {
                            int pre_r = first(new_p);
                            int pre_c = second(new_p);
                            if (previous[pre_r][pre_c%cols] == p) first_step = true;
                            else {
                                new_p = previous[pre_r][pre_c%cols];
                            }
                        }
                        p = new_p;
                        return;
                    }       
                }
            } 
        }
        return;
    }

    void dodge(Pos& p) 
    {
        
    }

    void missile(const Pos& p)  
    {

    }
    /**
     * Play method.
     * 
     * This method will be invoked once per each round.
     * You have to read the board here to place your actions
     * for this round.
     *
     */     
    virtual void play () 
    {
        if(round()==0) {
            all_dirs = {FAST_UP, FAST_DOWN, FAST,
                        UP,      DOWN,      DEFAULT,
                        SLOW_UP, SLOW_DOWN };
        }
        else {
            // For each of my cars
            for (Car_Id cid = begin(me()); cid != end(me()); ++cid) {
                Car c = car(cid);
                if (c.alive) { // if the car is alive and
                    Pos p = c.pos;
                    BFS(p,WATER_BONUS);
                    move(cid,p-c.pos);
                }
            }
        }
    }
};


/**
 * Do not modify the following line.
 */
RegisterPlayer(PLAYER_NAME);

