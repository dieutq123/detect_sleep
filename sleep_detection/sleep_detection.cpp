#include <dlib/opencv.h>
#include <opencv2/highgui/highgui.hpp>
#include <dlib/image_processing/frontal_face_detector.h>
#include <dlib/image_processing/render_face_detections.h>
#include <dlib/image_processing.h>
#include <dlib/gui_widgets.h>
#include <dlib/image_io.h>

#include <time.h>

#include <stdio.h>
#include <iostream>
using namespace dlib;
using namespace std;
using namespace cv;

image_window win;
shape_predictor sp;
std::vector<cv::Point> righteye;
std::vector<cv::Point> lefteye;
char c;
cv::Point p;
float times,timed;
float tmp_close = 0, tmp_open = 0, time_close = 0;
float end_close = 0, end_open = 0; 
float time_open = 0;
int i = 0, j = 0 , bl = 0, err = 0;
 
double compute_EAR(std::vector<cv::Point> vec)
{
    double a = cv::norm(cv::Mat(vec[1]), cv::Mat(vec[5]));
    double b = cv::norm(cv::Mat(vec[2]), cv::Mat(vec[4]));
    double c = cv::norm(cv::Mat(vec[0]), cv::Mat(vec[3]));
    
    double ear = (a + b) / (2.0 * c);
    return ear;
}

int main()
{
    try {
        cv::VideoCapture cap("../nam-thang-sang.mp4");

        if (!cap.isOpened()) {
            cerr << "Unable to connect to camera" << endl;
            return 1;
        }
        cap.set(CAP_PROP_FRAME_WIDTH, 640); //use small resolution for fast processing
        cap.set(CAP_PROP_FRAME_HEIGHT, 480);
        
        // Load face detection and deserialize  face landmarks model.
        frontal_face_detector detector = get_frontal_face_detector();

        deserialize("../../shape_predictor_68_face_landmarks.dat") >> sp;

        // Grab and process frames until the main window is closed by the user.
        const clock_t start = clock();
        while (!win.is_closed()) {
            // Grab a frame
            cv::Mat temp;
            if (!cap.read(temp)) {
                break;
            }

            cv_image<bgr_pixel> cimg(temp);
            full_object_detection shape;
            
            // Detect faces

            std::vector<rectangle> faces = detector(cimg);
            win.clear_overlay();
            win.set_image(cimg);
            
            // Find the pose of each face.
            if (faces.size() > 0) {
                shape = sp(cimg, faces[0]); //work only with 1 face

                for (int b = 36; b < 42; ++b) {
                    p.x = shape.part(b).x();
                    p.y = shape.part(b).y();
                    lefteye.push_back(p);
                }
                for (int b = 42; b < 48; ++b) {
                    p.x = shape.part(b).x();
                    p.y = shape.part(b).y();
                    righteye.push_back(p);
                }
                
                // cout << "left eye:" << lefteye << endl;
                //Compute Eye aspect ration for eyes
                double right_ear = compute_EAR(righteye);
                double left_ear = compute_EAR(lefteye);
                
                //if the avarage eye aspect ratio of lef and right eye less than 0.2, the status is sleeping.
                // if ((right_ear + left_ear) / 2 < 0.2) {
                //     const clock_t tmp_close = clock();

                //     if (end_open == 0) {
                //         time_close = float(tmp_close - start) / 1000000;
                //     } else {
                //         time_close = float(tmp_close - end_open) / 1000000;
                //     }
                   
                //     cout << "Close time :" << time_close << endl;

                //     if ( time_close > 2) {
                //         // cout << "Close time :" << time_close << endl;
                //         printf("\a");
                //         // win.add_overlay(dlib::image_window::overlay_rect(faces[0], rgb_pixel(255, 0, 0), "Warning"));
                //     }
                //     const clock_t close = clock();
                //     end_close = close;
                // } else {
                //     const clock_t tmp_open = clock();
                //     if ( end_close != 0) {
                //         float time_open = float(tmp_open - end_close) / 1000000;
                //          i++;
                //         cout << "count "<< i <<endl;
                //         cout << "Open time :" << time_open << endl;
                //         if ( time_open > 0.4) { // Real open eye
                //             end_open = tmp_open;
                //             // cout << "Open" << endl;
                //             // win.add_overlay(dlib::image_window::overlay_rect(faces[0], rgb_pixel(255, 255, 255), "Not sleeping"));
                //         } else { //
                //             cout << "Blink" << endl;
                //         }
                //     } else {
                //         end_open = tmp_open;
                //     }     
                // }
                // if ((right_ear + left_ear) / 2 < 0.2) {
                //     if ( j > 4 && err > 1) {
                //         i = 0;
                //     }
                //     i++;
                //     bl++;
                //     const clock_t tmp_close = clock();
                //     float time = float(tmp_close - tmp_open) /  1000000;
                //     cout << "time" << time << endl;
                //     cout << "Open   " << j << endl;
                //     cout << "close   " << i << endl;
                //     // cout << "close        " << i << endl;
                //     if ( i >= 40) {
                //         cout << "Sleep" << endl;
                //         win.add_overlay(dlib::image_window::overlay_rect(faces[0], rgb_pixel(255, 0, 0), "Warning"));
                //     } else
                //     {
                //         win.add_overlay(dlib::image_window::overlay_rect(faces[0], rgb_pixel(0, 0, 255), "CLose"));
                //     }
                //     err = 0;
                // } else {
                //     const clock_t tmp_open = clock();
                //     if( bl > 2 ) {
                //         j = 0;
                //     }
                //     err++;
                //     cout << "error   " << err << "j "<< j<< endl;
                //     j++;

                //     if (bl <= 8 && bl > 0) {
                //         cout << "blink" << endl;
                //     }
                //     bl = 0;
                //     win.add_overlay(dlib::image_window::overlay_rect(faces[0], rgb_pixel(255, 255, 255), "Not bleeping"));
                // }

                if ((right_ear + left_ear) / 2 < 0.2) {
                    const clock_t tmp_close = clock();
                    if ( end_close == 0) time_close = 0;
                    else time_close = time_close + float(tmp_close - end_close) / 1000000;
                    cout << "Sleep " <<  time_close << endl;
                    if (time_close > 2) {
                        time_open = 0;
                        printf("\a");
                        cout << "Sleep" << endl;
                        win.add_overlay(dlib::image_window::overlay_rect(faces[0], rgb_pixel(255, 0, 0), "Warning"));
                    } else {
                        win.add_overlay(dlib::image_window::overlay_rect(faces[0], rgb_pixel(0, 0, 255), "CLose"));
                    }
                    end_close = tmp_close;
                } else {
                    cout << "Sleep ---- open" <<  time_close << endl;
                    const clock_t tmp_open = clock();
                    if ( end_open == 0) time_open = 0;
                    else time_open = time_open + float(tmp_open - end_open) / 1000000;
                    cout << "Open" <<  time_open << endl;
                    if ( time_open > 0.1 || time_close <= 0.4) {
                        time_close = 0;
                        end_close = 0;
                    }

                    win.add_overlay(dlib::image_window::overlay_rect(faces[0], rgb_pixel(255, 255, 255), "Not bleeping"));
                    end_open = tmp_open;
                }
                
                righteye.clear();
                lefteye.clear();

                win.add_overlay(render_face_detections(shape));

                c = (char)waitKey(30);
                if (c == 27)
                    break;
            } else {
                if (time_close != 0) {
                    printf("\a");
                    cout << "warning " << endl;
                }       
            }
        }
    }
    catch (serialization_error& e) {
        cout << "Check the path to dlib's default face landmarking model file to run this example." << endl;
        cout << "You can get it from the following URL: " << endl;
        cout << "   http://dlib.net/files/shape_predictor_68_face_landmarks.dat.bz2" << endl;
        cout << endl
             << e.what() << endl;
    }
    catch (exception& e) {
        cout << e.what() << endl;
    }
}