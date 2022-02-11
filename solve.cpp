#include <bits/stdc++.h>
using namespace std;

#define GRID(x)     vector<vector<x>>
#define MP(x,y)     make_pair(x, y)
#define PII         pair<int,int>
#define ERR         1e-9

int n, m, k;
int tim;
GRID(bool) obstacle;
GRID(double) p;
int edge_vec_sz = 4, corn_vec_sz = 5;
int edge_vecx[] = {+1,  0, -1,  0};
int edge_vecy[] = { 0, +1,  0, -1};
int corn_vecx[] = {+1, +1, -1, -1, 0}; // a cell belongs to its own corner neighbour 
int corn_vecy[] = {-1, +1, -1, +1, 0}; // as casper can choose to stay in its cell 



double getMaxProb() {
    double ret = 0;
    for(int i = 0; i < n; i++) for(int j= 0; j < m; j++)
        ret = max(ret, p[i][j]);
    return ret;
}

void scaleUp(GRID(double) &vec) {
    for(int i = 0; i < n; i++) for(int j = 0; j < m; j++)
        vec[i][j] *= 100;
}

/*
Return format ->
    *..    | 0.345 0.345 0.345
    ..*    | 0.345 0.345 0.345
    C..    | 0.345 0.345 0.345 
*/
string getGrid() {
    string ret;
    char obs = '*', free_cell = '.', casper = 'C';
    double max_p = getMaxProb();

    for(int i = 0; i < n; i++) {
        string str = "\t";
        for(int j = 0; j < m; j++) 
            str += (obstacle[i][j] ? obs: ( (p[i][j]==max_p)?casper:free_cell) );
    
        str += "\t|\t";
        for(int j = 0; j < m; j++) {
            stringstream ss;
            ss << setprecision(3) << setw(6) << fixed << p[i][j];
            str += ss.str() + " ";
        }
        str += "\n";
        ret += str;
    }
    return ret;
}

void initGrid() {
    int free_cells = n*m - k;
    double prob = 1.0/free_cells;    
    for(int i = 0; i < n; i++)
        for(int j = 0; j < m; j++)
            p[i][j] = (!obstacle[i][j] ? prob:0);

    scaleUp(p);
}

PII getPosition() {
    PII pos = MP(-1, -1);
    double maxp = getMaxProb();
    for(int i = 0; i < n; i++) for(int j = 0; j < m; j++)
        if(p[i][j] == maxp) pos = MP(i, j);
    return pos;
}

double getProbSum() {
    double ret = 0;
    for(int i = 0; i < n; i++) for(int j = 0; j < m; j++)
        ret += p[i][j];
    return ret;
}

/**
 * @brief Checks whether r,c is a valid destination to move to
 * 
 */
inline bool isValidNext(int r, int c) {
    return r>=0 and r<n and c>=0 and c<m and !obstacle[r][c];
}

/**
 * @brief Checks whether cell r,c is within 3x3 square range centered from x,y
 * i.e. checks whether r,c is neighbour of x,y
 */
inline bool isNeighbour(int r, int c, int x, int y) {
    return (abs(r-x) + abs(c-y) <= 1) or (abs(r-x)==1 and abs(c-y)==1);
}

GRID(double) getNextInTime() {
    GRID(double) newp(n, vector<double>(m, 0));
    
    for(int i = 0; i < n; i++) {
        for(int j = 0; j < m; j++) {
            if(obstacle[i][j])
                continue;

            double edge_p = 0.9, corn_p = 1 - edge_p;
            int edge_neigh_count = 0, corn_neigh_count = 0;

            for(int k = 0; k < edge_vec_sz; k++)
                edge_neigh_count += isValidNext(i+edge_vecx[k], j+edge_vecy[k]);

            for(int k = 0; k < corn_vec_sz; k++)
                corn_neigh_count += isValidNext(i+corn_vecx[k], j+corn_vecy[k]);

            // corner case
            if(edge_neigh_count == 0) {
                edge_p = 0;
                corn_p = 1.0/corn_neigh_count;
            }
            else {
                edge_p /= edge_neigh_count;
                corn_p /= corn_neigh_count;
            }

            for(int k = 0; k < edge_vec_sz; k++) {
                int ni = i + edge_vecx[k];
                int nj = j + edge_vecy[k];
                if(isValidNext(ni, nj)) 
                    newp[ni][nj] += edge_p*p[i][j];
            }
            for(int k = 0; k < corn_vec_sz; k++) {
                int ni = i + corn_vecx[k];
                int nj = j + corn_vecy[k];
                if(isValidNext(ni, nj))
                    newp[ni][nj] += corn_p*p[i][j];
            }
        }
    }
    scaleUp(newp);
    return newp;
}

GRID(double) getNextInEvidence(int r, int c, bool sensorBlipped) {
    GRID(double) ret(n, vector<double>(m, 0));
    double sum = 0;
    for(int i = 0; i < n; i++) {
        for(int j = 0; j < m; j++) {
            if(isNeighbour(i, j, r, c)) 
                ret[i][j] = (sensorBlipped ? 0.85*p[i][j] : 0.15*p[i][j]);
            else 
                ret[i][j] = (sensorBlipped ? 0.15*p[i][j] : 0.85*p[i][j]);
            
            sum += ret[i][j];
        }
    }
    for(int i = 0; i < n; i++) for(int j = 0; j < m; j++)
        ret[i][j] /= sum;

    scaleUp(ret);
    return ret;
}

int main() {
    freopen("in.txt", "r", stdin);
    freopen("out.txt", "w", stdout);

    cin >> n >> m >> k;
    
    obstacle.resize(n, vector<bool>(m, false));
    p.resize(n, vector<double>(m, 0));

    for(int i = 0; i < k; i++) {
        int r, c;
        cin >> r >> c;
        obstacle[r][c] = true;
    }

    initGrid();

    cout << "Time " << tim << endl;
    cout << getGrid() << endl;
    double total = getProbSum();
    cout << "Total = " << setprecision(3) << fixed << total << endl << endl; 

    string q;
    while(cin >> q) {
        if(q == "R") {
            int r, c, f;
            cin >> r >> c >> f;
            p = getNextInTime();
            p = getNextInEvidence(r, c, f);
            tim++;
            double total = getProbSum();
            
            cout << "Time " << tim << endl;
            cout << getGrid() << endl;
            cout << "Total = " << setprecision(3) << fixed << total << endl << endl; 
        }
        else if(q == "T") {
            p = getNextInTime();
            tim++;

            cout << "Time " << tim << endl;
            cout << getGrid() << endl;
            cout << "Total = " << setprecision(3) << fixed << total << endl << endl; 
        }
        else if(q == "C") {
            PII pos = getPosition();
            cout << "Most likely position: " << pos.first << ", " << pos.second << endl << endl;
        }
        else if(q == "Q") {
            break;
        }
        else {
            cout << "Invalid query. Try again" << endl;
        }
    }

    return 0;   
}
