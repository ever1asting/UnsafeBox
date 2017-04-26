#ifndef LINKPAIR_H
#define LINKPAIR_H


class LinkPair
{
public:
    LinkPair(int, int, int, int, int infect = 0, int w = 3);
    LinkPair(const LinkPair&);

    int fir;
    int sec;
    int state;
    int arg;
    int width; // the width of the pen
    int isInfect;
};

#endif // LINKPAIR_H
