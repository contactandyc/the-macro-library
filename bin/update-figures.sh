./speed_test_final $1 $2
macro-plot.py -o ../../../images/speed_test_final_bar.png
macro-plot2.py 1 -o ../../../images/speed_test_final_1.png
macro-plot2.py 2 -o ../../../images/speed_test_final_2.png
macro-plot2.py -o ../../../images/speed_test_final_3.png
./speed_test $1 $2
macro-plot.py -o ../../../images/speed_test_bar.png
macro-plot2.py 1 -o ../../../images/speed_test_1.png
macro-plot2.py -o ../../../images/speed_test_2.png
./speed_test2 $1 $2
macro-plot.py -o ../../../images/speed_test2_bar.png
macro-plot2.py -o ../../../images/speed_test2_1.png
./speed_test_class $1 $2
macro-plot.py -o ../../../images/speed_test_class_bar.png
macro-plot2.py -o ../../../images/speed_test_class_1.png
macro-plot2.py 1 -o ../../../images/speed_test_class_2.png
./speed_test_class2 $1 $2
macro-plot.py -o ../../../images/speed_test_class2_bar.png
macro-plot2.py -o ../../../images/speed_test_class2_1.png
macro-plot2.py 1 -o ../../../images/speed_test_class2_2.png
./speed_test_class3 $1 10
macro-plot.py -o ../../../images/speed_test_class3_bar.png
macro-plot2.py -o ../../../images/speed_test_class3_1.png
macro-plot2.py 1 -o ../../../images/speed_test_class3_2.png
