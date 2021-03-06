#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <iostream>
#include <stdio.h>
#include <queue>
#include <vector>
#include <climits>

int isValid(int i, int j, cv::Mat img)
{
  if( i<0 || j<0 || i >= img.rows-1 || j >= img.cols-1 )
    return 0;
  else
    return 1;
}


cv::Point find_point( cv::Mat b, char ch)
{
  cv::Point temp;
  temp.x=0;
  temp.y=0;
  int count=0;

  if(ch=='s')
    {
      for(int i=0; i<b.rows; i++)
	{
	  for(int j=0; j<b.cols; j++)
	    {
	      if(b.at<cv::Vec3b>(i,j)[2]==255 && b.at<cv::Vec3b>(i,j)[1]==0 && b.at<cv::Vec3b>(i,j)[0]==0)
		{
		  temp.x+=j;
		  temp.y+=i;
		  count++;
		}
   
	    }
	}
      temp.x/=count;
      temp.y/=count;
    }

  if(ch=='e')
    {
      for(int i=0; i<b.rows; i++)
	{
	  for(int j=0; j<b.cols; j++)
	    {

	      if(b.at<cv::Vec3b>(i,j)[2]==0 && b.at<cv::Vec3b>(i,j)[1]==255 && b.at<cv::Vec3b>(i,j)[0]==0)
		{
		  temp.x+=j;
		  temp.y+=i;
		  count++;
		}
   
	    }
	}
      temp.x/=count;
      temp.y/=count;
    }

  return temp;
}

struct node
{
  int visited = 0;  
  int gVal=INT_MAX/2;
  int hVal=INT_MAX/2;
  int fVal=INT_MAX;
  cv::Point parent;
  cv::Point self;
};


struct Compare_
{

  bool operator()(const node& l, const node& r)
  {
    return l.gVal>r.gVal;
  }
};

struct Compare
{

  bool operator()(const node& l, const node& r)
  {
    return l.fVal>r.fVal;
  }
};

class PathPlanner
{
private:

  cv::Point start;
  cv::Point end;
  cv::Mat a;
  char plan_method;
  int path_length;
  int points_traversed;
  std::vector<cv::Point> path;

public:
  PathPlanner(cv::Point Start, cv::Point End , cv::Mat img , char Plan_method)
  {
    start=Start;
    end=End;
    a=img;
    plan_method=Plan_method;
  }

  void getpath();

};

void PathPlanner::getpath()
{ 
  path_length=0;
  points_traversed=0;
  std::vector<std::vector<node> > graph(a.rows,std::vector<node>(a.cols));
	
  for(int i=0; i<a.rows; i++)
    {
      for(int j=0; j<a.cols; j++)
	{
	  graph[i][j].self.y=i;
	  graph[i][j].self.x=j;
	}
    }


	
  std::priority_queue<node, std::vector<node>, Compare> Q;
	

  for(int i=0; i<a.rows; i++)
    {
      for(int j=0; j<a.cols; j++)
	{
	  graph[i][j].self.y=i;
	  graph[i][j].self.x=j;
	}
    }


  if(plan_method=='a')
    {
      for( int i=0;i<a.rows;i++)
	{
	  for(int j=0;j<a.cols;j++)
	    {
	      graph[i][j].hVal=7*abs(end.x-j)+7*abs(end.y-i);
	    }
	}
    }

  if(plan_method=='d')
    {
      for( int i=0;i<a.rows;i++)
	{
	  for(int j=0;j<a.cols;j++)
	    {
	      graph[i][j].hVal=0;
	    }
	}
    }

	



  graph[start.y][start.x].gVal=0;
  graph[start.y][start.x].fVal=0;

  Q.push(graph[start.y][start.x]);

  while((Q.top().self.x != end.x || Q.top().self.y != end.y) && !Q.empty())
    {
      for(int i=Q.top().self.y, u=-1; u<=1; u++)
	{
	  for(int j=Q.top().self.x, v=-1; v<=1; v++)
	    {
	      if(isValid(i+u,j+v,a))              
		{
		  if(graph[i+u][j+v].visited == 0 && !((a.at<cv::Vec3b>(i+u,j+v)[0]==255) && (a.at<cv::Vec3b>(i+u,j+v)[1]==255) && (a.at<cv::Vec3b>(i+u,j+v)[2]==255)))
		    {
		      if(u==0 && v==0)
			{
			  graph[i+u][j+v].fVal=graph[i+u][j+v].gVal + graph[i+u][j+v].hVal;
			  continue;
			}
		      if(abs(u)==abs(v))
			{
							
			  if(Q.top().gVal +14 < graph[i+u][j+v].gVal )
			    {
			      graph[i+u][j+v].gVal = Q.top().gVal + 14;
			      graph[i+u][j+v].parent=Q.top().self;
			      graph[i+u][j+v].fVal=graph[i+u][j+v].gVal + graph[i+u][j+v].hVal;
			      Q.push(graph[i+u][j+v]);
			    }
			}
		      if(abs(u)!=abs(v))
			{
			  if(Q.top().gVal +10 < graph[i+u][j+v].gVal )
			    {
			      graph[i+u][j+v].gVal = Q.top().gVal + 10;
			      graph[i+u][j+v].parent=Q.top().self;
			      graph[i+u][j+v].fVal=graph[i+u][j+v].gVal + graph[i+u][j+v].hVal;
			      Q.push(graph[i+u][j+v]);
			    }
			}


		    }
		}
	    }
	}
      graph[Q.top().self.y][Q.top().self.x].visited=1;
      points_traversed++;
      Q.pop();
    }
  cv::Point temp=graph[end.y][end.x].self;
  while(temp!=graph[start.y][start.x].self)
    {
      a.at<cv::Vec3b>(temp.y, temp.x)[2]=255;
      a.at<cv::Vec3b>(temp.y, temp.x)[1]=255;
      path.push_back (temp);
      temp=graph[temp.y][temp.x].parent;
      path_length++;
    }

  printf("Path length=%d\n",path_length);
  printf("Total points traversed=%d\n",points_traversed);

  cv::imshow("Shortest_path", a);
  cv::waitKey(0);

}





int main(int argc, char **argv)
{
  cv::Mat b=cv::imread(argv[1],1);
  cv::Point start,end;
 
  start=find_point(b,'s');
  end=find_point(b,'e');

  char choice;	

  printf("Choose the path planning algorithm-\nFor Dijkstra enter 'd'\nFor A-star enter 'a'\n");
  scanf("%c",&choice);

  PathPlanner spath(start,end,b,choice);
  spath.getpath();

  return 0;

}
