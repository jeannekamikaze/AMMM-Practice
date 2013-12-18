generate_class_images = function(filename, number_of_classes = 2) { 
    source("postprocess_2_class.R");
    source("postprocess_3_class.R");

    if (number_of_classes == 2) generate_2_class_images(filename);
    if (number_of_classes == 3) generate_3_class_images(filename);
}