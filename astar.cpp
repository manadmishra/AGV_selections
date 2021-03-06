#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <iostream>
#include <stdio.h>
#include <queue>
#include <vector>
#include<climits>

struct node
{
  int visited = 0;  
  int gVal=INT_MAX/2;
  int hVal=INT_MAX/2;
  int fVal=INT_MAX;
  cv::Point parent;
  cv::Point self;
};


int isValid(int i, int j, cv::Mat img)
{
  if( i<0 || j<0 || i >= img.rows-1 || j >= img.cols-1 )
    return 0;
  else
    return 1;
}


struct Compare
{

  bool operator()(const node& l, const node& r)
  {
    return l.fVal>r.fVal;
  }
};

int main(int argc, char **argv)
{

  cv::Mat a=cv::imread(argv[1],1);
  int path_length=0, points_traversed=0;
  std::priority_queue<node, std::vector<node>, Compare > Q;
  std::vector<std::vector<node> > graph(a.rows,std::vector<node>(a.cols));
  
  cv::Point start,end;
  start.x=0;
  start.y=0;
  end.x=0;
  end.y=0;
  
  

  int count1=0, count2=0;

  for(int i=0; i<a.rows; i++)
    {
      for(int j=0; j<a.cols; j++)
	{
	  if(a.at<cv::Vec3b>(i,j)[2]==255 && a.at<cv::Vec3b>(i,j)[1]==0 && a.at<cv::Vec3b>(i,j)[0]==0)
	    {
	      start.x+=j;
	      start.y+=i;
	      count1++;
	    }
	  else if(a.at<cv::Vec3b>(i,j)[2]==0 && a.at<cv::Vec3b>(i,j)[1]==255 && a.at<cv::Vec3b>(i,j)[0]==0)
	    {
	      end.x+=j;
	      end.y+=i;
	      count2++;
	    }
	}
    }
  start.x/=static_cast<float>(count1);
  start.y/=static_cast<float>(count1);
  end.x/=static_cast<float>(count2);
  end.y/=static_cast<float>(count2);

  for(int i=0; i<a.rows; i++)
    {
      for(int j=0; j<a.cols; j++)
	{
	  graph[i][j].self.y=i;
	  graph[i][j].self.x=j;
	}
    }

  for( int i=0;i<a.rows;i++)
    {
      for(int j=0;j<a.cols;j++)
	{
	  graph[i][j].hVal=7*abs(end.x-j)+7*abs(end.y-i);
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
		      else
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
      a.at<cv::Vec3b>(Q.top().self.y, Q.top().self.x)[0]=140;
  cv::imshow("Shortest_Path", a);
  cv::waitKey(1);
      Q.pop();
    }
  cv::Point temp=graph[end.y][end.x].self;
  while(temp!=graph[start.y][start.x].self)
    {
      a.at<cv::Vec3b>(temp.y, temp.x)[2]=255;
      a.at<cv::Vec3b>(temp.y, temp.x)[1]=255;
      temp=graph[temp.y][temp.x].parent;
      path_length++;
    }

  printf("Path length=%d\n",path_length);
  printf("Total points traversed=%d\n",points_traversed);

  cv::imshow("Shortest_path", a);
  cv::waitKey(0);

}






