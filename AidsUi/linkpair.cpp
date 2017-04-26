#include "linkpair.h"

LinkPair::LinkPair(int f, int s, int sta, int a, int infect, int w)
    : fir(f), sec(s), state(sta), arg(a), isInfect(infect), width(w)
{

}

LinkPair::LinkPair(const LinkPair& lp)
{
    fir = lp.fir;
    sec = lp.sec;
    arg = lp.arg;
    state = lp.state;
    width = lp.width;
    isInfect = lp.isInfect;
}
