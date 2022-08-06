#include <thread>
#include <iostream>
#include <chrono>
#include <vector>
#include <opencv2/videoio/videoio.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/objdetect.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/video/video.hpp>
#include <memory>
#include <filesystem>
using namespace std;
using namespace cv;
//Constant Variables


//Constant Variables
//Variables
int i = 0;
int img_ctr = 0;
Mat Image;
int Temp_Stack_Size = 15;
int width = 300;
int height = 300;
bool which_Temp_Stack = 0;

class Temporary_Stacks
{
public:
    std::vector<cv::Mat>Temp_Stack0;
    std::vector<cv::Mat>Temp_Stack1;
};
std::shared_ptr<Temporary_Stacks>Temporary_Stacks_ptr = std::make_shared<Temporary_Stacks>();

//Variables
VideoWriter Output("Output.avi", VideoWriter::fourcc('M', 'J', 'P', 'G'), 10, Size(width, height), true);
//Functions

//Functions
using namespace std;
using namespace cv;
using namespace std::this_thread;

cv::VideoCapture Cap_Foscam(0);


int Save(std::shared_ptr<Temporary_Stacks>Temporary_Stacks_ptr, bool which_stack)
{
    if (!which_stack)//Now the Temp_stack0 is being filled.
    {
        for (int y = 0; y <= (Temporary_Stacks_ptr->Temp_Stack0.size() - 1); y++)
        {
            cout << "HOPES0\n";
            Output.write(Temporary_Stacks_ptr->Temp_Stack0[y]);
        }
    Temporary_Stacks_ptr->Temp_Stack0.clear();
    }


    if (which_stack)//Now the Temp_stack1 is being filled.
    {
        for (int y = 0; y <= (Temporary_Stacks_ptr->Temp_Stack1.size() - 1); y++)
        {
            cout << "HOPES1\n";
            Output.write(Temporary_Stacks_ptr->Temp_Stack1[y]);
        }
    Temporary_Stacks_ptr->Temp_Stack1.clear();
    }
    if (which_stack == 0)
    {
        return 1;
    }
    else
    {
        return 0;
    }

}


void Get_New_frame()
{
    using namespace std::literals::chrono_literals;
    while (1)
    {
        Cap_Foscam.grab();
        std::this_thread::sleep_for(0.05s);
    }
}

void Display_Images_Real_Time()
{
    while (1)
    {
        if (Cap_Foscam.isOpened())
        {
            if (Cap_Foscam.retrieve(Image))
            {
                imshow("Foscam Real Time Feed", Image);
                waitKey(1);
            }
        }
        else
        {
            cout << "Connecting...\n";
        }
    }

}

int main()
{
    int width = Cap_Foscam.get(3);
    int height = Cap_Foscam.get(4);
    Temporary_Stacks_ptr->Temp_Stack0.reserve(Temp_Stack_Size);
    Temporary_Stacks_ptr->Temp_Stack1.reserve(Temp_Stack_Size);

    std::thread NEWEST_FRAME(Get_New_frame);
    std::thread Real_Time_Video(Display_Images_Real_Time);
    VideoWriter Output("Output.avi", VideoWriter::fourcc('M', 'J', 'P', 'G'), 10, Size(width, height), true);

    while (1)
    {
        img_ctr++;
        while (!Cap_Foscam.isOpened())
        {
            cv::VideoCapture Cap_Foscam(0);
            std::cout << "Connecting...\n";
        }

        while (Cap_Foscam.isOpened())
        {
            std::cout << img_ctr << endl;
            img_ctr++;//every 10th frame should go though object detection
            

            if (img_ctr >= Temp_Stack_Size)
            {
                which_Temp_Stack = Save(Temporary_Stacks_ptr, which_Temp_Stack);

                //for (int y = 0; y <= (Temporary_Stacks_ptr->Temp_Stack0.size() - 1); y++)
                //{
                //    cout << "HOPES1\n";
                //    Output.write(Temporary_Stacks_ptr->Temp_Stack0[y]);
                //}

                //THE COMMENTED CODE ABOVE(LINE 144 TO 148) CAN BE USED TO SEE IF THE IMAGES ARE LOADED INTO THE CLASS, THIS IS ONLY TO SEE FOR YOURSELF THAT THE PUSH_BACK COMMAND WORKES//

                std::cout << "Saved\n";
                img_ctr = 0;
            }
            Cap_Foscam.retrieve(Image);
            if (!which_Temp_Stack)
            {
                Temporary_Stacks_ptr->Temp_Stack0.push_back(Image.clone());
            }
            else
            {
                Temporary_Stacks_ptr->Temp_Stack1.push_back(Image.clone());
                //cout << "TEST";
            }


        }





       

    }
    return 0;
}

