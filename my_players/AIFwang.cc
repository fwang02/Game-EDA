
#include "Player.hh"

using namespace std;


/**
 * Write the name of your player and save this file
 * as AI<name>.cc
 */
#define PLAYER_NAME Fwang





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
    typedef vector<int> VI;
    typedef vector<VI>  VVI;
    typedef vector<Pos> VP;
    typedef vector<VP> VVP;
    typedef vector<Dir> VD;

    VD all_dirs;
    Dir d1 = {-1,-1};
    Dir d2 = {-1,-1};

    void generate_matrix()
    {
        /*int rows = number_rows();
        int cols = number_universe_columns();
        //map = VVC(rows,VC(cols));
        for(int i = 0; i < rows; ++i) {
            for(int j = 0; j < cols; ++j) {
                Cell c = cell(Pos(i,j));
                if(c.type == TYRE) map[i][j] = 'T';
                else if(c.type == EMPTY) map[i][j] = '.';
                else if(c.type == CAR) map[i][j] = 'C';
                else if(c.type == MISSILE) map[i][j] = 'm';
                else if(c.type == WATER_BONUS) map[i][j] = 'W';
                else if(c.type == MISSILE_BONUS) map[i][j] = 'M';
                else if(c.type == GAS_BONUS) map[i][j] = 'G';
            }
        }*/
    }

    void update_matrix()
    {
        
    }

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

    Dir BFS(const Pos& p,const CType& elem)
    {
        int rs = first(p);
        int cs = second(p);
        int c_mat = cs%number_window_columns();
        int rows = number_rows();
        int cols = number_window_columns();
        VVI dist(rows, VI(cols, 1e9));
        VVP previous(rows, VP(cols));
        queue<Pos> q;

        dist[rs][c_mat] = 0;
        q.push(Pos(rs,cs));

        while (not q.empty()) {
            Pos curr = q.front();
            int r = first(curr);
            int c = second(curr);
            int cw = number_window_columns();
            
            q.pop();

            if(cell(curr).type == elem) {
                vector<Pos> shortestPath;
                Pos current = curr;

                while (current != p) {
                    shortestPath.push_back(current);
                    current = previous[first(current)][second(current)%cw];
                }

                if (not shortestPath.empty()) {
                    return shortestPath.back()-p; 
                } else {
                    return Dir(-1, -1);
                }
            }

            for(Dir d: all_dirs) {
                Pos new_p = curr+d;
                int rr = first(new_p);
                int cc = second(new_p);
                if(within_window(new_p,round()+1) and can_move(curr,d)) {
                    if(has_bonus(new_p)) return d;

                    int new_dist = dist[r][c%cw];
                    if(fast(d)) new_dist += 2;
                    else if(normal(d) or slow(d)) new_dist++;
                    
                    if(new_dist < dist[rr][cc%cw]) {
                        dist[rr][cc%cw] = new_dist;
                        previous[rr][cc%cw] = curr; 
                        q.push(new_p);                       
                    }
                }
            } 
        }
        return Dir(-1,-1);
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
                    if(cid == begin(me())) {
                        if(d1 != Dir(-1,-1)) move(cid,d1);
                        d1 = BFS(p,WATER_BONUS);
                    }
                    else {
                        if(d2 != Dir(-1,-1)) move(cid,d2);
                        d2 = BFS(p,WATER_BONUS);
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

