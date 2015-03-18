#ifndef DATASTRUCTURE_H
#define DATASTRUCTURE_H

#endif // DATASTRUCTURE_H
typedef struct TrackPoint
{
    int id;
    int x;
    int y;
}
TrackPoint;
TrackPoint  trackPoint( int id,int x, int y )
{
    TrackPoint p;
    p.id = id;
    p.x = x;
    p.y = y;

    return p;
}
