
#include "Player.hh"

using namespace std;


/**
 * Write the name of your player and save this file
 * as AI<name>.cc
 */
#define PLAYER_NAME Fwang_88
//bfs+dodge+safe




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

    bool safe(const Pos& p,const Pos& np)
    {   
        if(np != Pos(-1,-1)) {
            CType t = cell(np).type;
            if(t == WATER_BONUS) {
                for(const Dir& d : all_dirs) {
                    if(d != np-p and within_window(np-d,round()+1)) {
                        if(cell(np-d).type == CAR or cell(np-d).type == MISSILE) return false;
                    }
                }
                return true;
            }
            else {
                if(cell(np-DEFAULT).type == MISSILE and within_window(np-DEFAULT,round()+1)) return false;
                if(cell(np-FAST).type == MISSILE and within_window(np-FAST,round()+1)) return false;
                return true;
            }
        }
        return false;
    }

    Pos BFS(const Pos& p,const CType& elem)
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
                        return new_p;
                    }       
                }
            } 
        }
        return Pos(-1,-1);
    }

    bool dodge(const Car_Id& cid,const Pos& p) 
    {   
        if(cell(p-FAST).type == MISSILE or cell(p-DEFAULT).type == MISSILE) {
            if(within_window(p+FAST_UP,round()+1) and can_move(p,FAST_UP)) {
                move(cid,FAST_UP);
                return true;
            }
            else if(within_window(p+FAST_DOWN,round()+1) and can_move(p,FAST_DOWN)) {
                move(cid,FAST_DOWN);
                return true;
            }
            else if(within_window(p+UP,round()+1) and can_move(p,UP)) {
                move(cid,UP);
                return true;
            }
            else if(within_window(p+DOWN,round()+1) and can_move(p,DOWN)) {
                move(cid,DOWN);
                return true;
            }
            else if(within_window(p+SLOW_UP,round()+1) and can_move(p,SLOW_UP)) {
                move(cid,SLOW_UP);
                return true;
            }
            else if(within_window(p+SLOW_DOWN,round()+1) and can_move(p,SLOW_DOWN)) {
                move(cid,SLOW_DOWN);
                return true;
            }
        }
        return false;
    }

    void missile(const Pos& p, const Car_Id& cid)  
    {
        for(int i = 1; i <= 5; ++i) {
            Cell c = cell(p+Pos(0,i));
            if( (i==1 and c.type == TYRE) or (c.type == CAR and player_of(c.cid) != me())) {
                shoot(cid); 
                return;
            }
        }
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
                    if(c.nb_miss>0) missile(c.pos,cid);
                    if(not dodge(cid,c.pos)) {
                        Pos new_p;
                        if(c.gas < 20) {
                            new_p = BFS(c.pos,GAS_BONUS);
                            if(new_p == Pos(-1,-1)) new_p = BFS(c.pos,WATER_BONUS);
                        }
                        else if(c.nb_miss == 0) {
                            new_p = BFS(c.pos,MISSILE_BONUS);
                            if(new_p == Pos(-1,-1)) new_p = BFS(c.pos,WATER_BONUS);
                        }
                        else {
                            new_p = BFS(c.pos,WATER_BONUS);
                        }

                        if(new_p != Pos(-1,-1) and safe(c.pos,new_p)) move(cid,new_p-c.pos);
                    }
                }
            }
        }
    }
};


/**
 * Do not modify the following line.
 */
RegisterPlayer(PLAYER_NAME);

