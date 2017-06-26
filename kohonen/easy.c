#include <GL/glut.h>
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<time.h>

/*function to add a city's X & Y Co-ordinate*/
int add_to_pointArray(int array_size,float x_value,float y_value);

/*function to add the city in tourArray*/
int add_to_tourArray(int array_size,int x,int y);

/*to find the total tour Distance*/
float tour_Distance(void);

/*to find the Distance between to cities*/
float Dist(int point_1,int point_2);

/*to show the Cities positions*/
void ShowPts(void);

/*to show the Route*/
void ShowTour(void);

/*to Draw the Routes and Cities position*/
void Draw(void);

/*initializing the window*/
void Initialize(void);

int max_X=500,max_Y=500;/*for window size*/
float width=0,height=0;

/*write the cities.txt and store in pointArray*/
int RandCities(int N);

/*write the tour.txt and store in tourArray*/
int write_tour_txt(int start,FILE *tour);

/*read the cities postion and store in ptArray*/
int read_cities_txt(char *city);

/*produces R randomly starting points*/
int RandStarts(int N,int R);

/*produces N randomly points*/
int RandCities(int N);
int NumPts=0;

/*array to store cities' position*/
float *ptArray;

/*array to store cities in sequence for the tour*/
int *tourArray;

/*pointer for file names*/
char *file1,*file2;

int RandCities(int N)
{
    FILE *city;
    if((city=fopen(file1,"w"))==0)
    {
        printf("\n%s file can not open.\n\n",file1);
        return 0;
    }
    int PtPtr=0;
    float x_ordinate=0,y_ordinate=0;
    for(PtPtr=1;PtPtr<=N;PtPtr++)
    {
        x_ordinate=1+rand()%max_X;
        y_ordinate=1+rand()%max_Y;
        fprintf(city,"%4d %8.2f %8.2f\n",PtPtr,x_ordinate,y_ordinate);
        add_to_pointArray(2*PtPtr,x_ordinate,y_ordinate);
    }
    fclose(city);
    return 1;
}

int RandStarts(int N,int R)
{
    FILE *ran;
    if((ran=fopen(file2,"w"))==0)
    {
        printf("\n%s file can not open.\n\n",file2);
        return 0;
    }
    int i=0,temp=0;
    for(i=1;i<=R;i++)
    {
        temp=1+rand()%N;
        fprintf(ran,"%d\n",temp);
    }
    fclose(ran);
    return 1;
}

int RandAproxTSTour(char *file1,char *file2)
{
    read_cities_txt(file1);/*read cities.txt file*/
    FILE *rand;
    rand=fopen(file2,"r");
    int starting_point=0;
    FILE *tour;
    if((tour=fopen("tour.txt","w"))==0)/*checking file exists or not*/
    {
        printf("\ntour.txt can not open.\n\n");
        return 0;
    }
    /*read random starting points from file and find the NN tour*/
    while(fscanf(rand,"%d",&starting_point)!=EOF)
    write_tour_txt(starting_point,tour);

    fclose(rand);
    fclose(tour);

    return 1;
}

int write_tour_txt(int start,FILE *tour)
{
    /*for each new starting point tourArray should be refreshed*/
    tourArray=realloc(tourArray,sizeof(int)*1);
    int PtPtr,index,track_point=0,t,check=0,next;
    next=start;
    float nearest_dist=0,temp=0,total_distance=0;
    fprintf(tour,"the route with starting point %d is:\n%d",start,start);

    /*find consecutive NN*/
    for(index=1;index<NumPts;index++)
    {
        nearest_dist=0;
        for(PtPtr=1;PtPtr<=NumPts;PtPtr++)
        {
            check=0;
            /*will check the repitation in the current tourArray*/
            for(t=1;t<index;t++)
            if(PtPtr==tourArray[t-1])
            {
                check=-1;
                break;
            }
            /*for no repitation check will remain zero*/
            if(check==0 && next!=PtPtr)
            {
                /*Eucledian distance between next & PtPtr*/
                temp=Dist(next,PtPtr);
                if(temp<nearest_dist || nearest_dist==0)
                {
                    nearest_dist=temp;
                    track_point=PtPtr;
                }
            }
        }
        /*update the tourArray*/
        add_to_tourArray(index+1,next,track_point);
        if(index%20==0)fprintf(tour,"\n");
        fprintf(tour,"-%d",track_point);
        next=track_point;
        total_distance=total_distance + nearest_dist;
    }
    add_to_tourArray(NumPts+1,next,start);
    temp=Dist(next,start);
    total_distance=total_distance + temp;
    fprintf(tour,"-%d",start);
    fprintf(tour,"\nThe total distance is %.2f",total_distance);
    fprintf(tour,"\n=============================================\n");
    return 1;
}
int read_cities_txt(char *fName)
{
    FILE *city;
    if((city=fopen(fName,"r"))==0)
    {
        printf("\ncities.txt file does not exist.\n\n");
        return 0;
    }
    int PtPtr=0;
    float x_ordinate=0,y_ordinate=0;
    char ch='\n';
    for(PtPtr=1; ;PtPtr++)
    {
        if(ch==' ')/*skip all the space character*/
        {
            while((ch=getc(city))==' ');
            putc(ch,city);
        }
        if(ch!='\n')/*if there is more data in a line*/
        {
            printf("\nline %d, in cities.txt input is invalid.\n\n",PtPtr-1);
            return 0;
        }
        fscanf(city,"%d",&NumPts);
        fscanf(city,"%f",&x_ordinate);
        fscanf(city,"%f",&y_ordinate);
        if((ch=getc(city))==EOF)break;
        if(NumPts!=PtPtr)
        {
            printf("\nCity number in cities.txt is incorrect\n\n");
            free(ptArray);
            return 0;
        }
        else
        add_to_pointArray(2*PtPtr,x_ordinate,y_ordinate);
    }
    if(NumPts==0)
    {
        printf("\ncities.txt is empty.\n\n");
        return 0;
    }
    fclose(city);
    return 1;
}

int add_to_pointArray(int array_size,float x_value,float y_value)
{

    ptArray=realloc(ptArray,sizeof(float)*array_size);//resize the pointArray
    ptArray[array_size-2]=x_value;
    ptArray[array_size-1]=y_value;
    return 0;
}

int add_to_tourArray(int array_size,int x,int y)
{

    tourArray=realloc(tourArray,sizeof(float)*array_size);//resize the tourArray
    tourArray[array_size-2]=x;
    tourArray[array_size-1]=y;
    return 0;
}

float Dist(int point_1,int point_2)
{
    float x,y;
    x=ptArray[2*point_1-2]-ptArray[2*point_2-2];/*x1-x2*/
    y=ptArray[2*point_1-1]-ptArray[2*point_2-1];/*y1-y2*/
    return sqrt(x*x+y*y);
}

float tour_Distance(void)
{
    int index=0;
    float Distance=0;
    for(index=0;index<NumPts;index++)
    Distance=Distance+Dist(tourArray[index],tourArray[index+1]);
    return Distance;
}

/*to show the Cities positions*/
void ShowPts()
{
    int index=0;
    glColor3f(1.0, 0.0, 0.0);
    glPointSize(3);
    glBegin(GL_POINTS);
    for(index=0;index<NumPts;index++)
    glVertex3f(ptArray[2*index]+width/23,ptArray[2*index+1]+height/12,0.0);
    glEnd();
}

/*to show the Route*/
void ShowTour()
{
    int index=0;
    glColor3f(1.0, 0.0, 0.0);
    glPointSize(3);
    glBegin(GL_POINTS);
    for(index=0;index<NumPts;index++)
    glVertex3f(ptArray[2*index]+width/2+width/23,ptArray[2*index+1]+height/12,0.0);
    glEnd();
    glColor3f(0.0, 0.0, 0.0);
    glBegin(GL_LINE_LOOP);
    for(index=0;index<NumPts;index++)
    glVertex3f(ptArray[2*tourArray[index]-2]+width/2+width/23,ptArray[2*tourArray[index]-1]+height/12,0.0);
    glEnd();
}
/*to Draw the Routes and Cities position*/
void Draw()
{
    glClear(GL_COLOR_BUFFER_BIT);
    ShowPts();
    ShowTour();
    glFlush();
}
/*initializing the window*/
void Initialize()
{
    glClearColor(1.0,1.0,1.0,0.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0,width,0.0,height,-1.0, 1.0);
}

/*Start of Main function*/
/*two  file names will be taken from command line*/
int main(int argc,char *argv[])
{
    int rand_city=0;
    file1=argv[1];
    file2=argv[2];
    printf("\nEnter the city no: ");
    scanf("%d",&NumPts);
    printf("\nEnter the number of random cities to generate: ");
    scanf("%d",&rand_city);
    if(NumPts<rand_city)
    {
        printf("\nR can not be greater than N.\n");
        return 0;
    }

    /*if text files can not open returns 0 and the main return 0*/
    if(RandCities(NumPts)==0)return 0;
    if(RandStarts(NumPts,rand_city)==0)return 0;
    /*for calculating processin time*/
    clock_t start,end;
    start=clock();
    RandAproxTSTour(file1,file2);
    end=clock();
    double t_elaps=(end-start)/rand_city;
    printf("\naverage process time with %d points is %.9lf\n",NumPts,t_elaps/CLOCKS_PER_SEC);

    /*for openGL implementation*/
    width=2.3*max_X;
    height=1.2*max_Y;
    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(width,height);
    glutInitWindowPosition(200, 200);
    glutCreateWindow("Travelling Salesman Problem");
    Initialize();
    glutDisplayFunc(Draw);
    glutMainLoop();
    free(ptArray);
    free(tourArray);
    return 0;
}/*End of Main Loop*/
