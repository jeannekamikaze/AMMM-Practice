 generate_3_class_images = function(filename) { 
    library(png);

    data <- read.table(paste(filename, ".mat", sep = ""), sep = "\t");
    data <- apply(data, MARGIN = c(1,2), FUN = as.numeric)

    image <- readPNG(source = paste(filename, ".png", sep = ""), native = FALSE);

    channels <- 4
    size <- sqrt(length(image) / channels);                

    image <- array(image, c(size, size, 4))[,,1:3];    

    pixels_class_0 <- matrix(image, nrow = size*size, ncol = 3);
    pixels_class_1 <- matrix(image, nrow = size*size, ncol = 3);
    pixels_class_2 <- matrix(image, nrow = size*size, ncol = 3);

    pixels_class_0[which(data != 0), ] <- c(0, 0, 0);
    pixels_class_1[which(data != 1), ] <- c(0, 0, 0);
    pixels_class_2[which(data != 2), ] <- c(0, 0, 0);

    writePNG(normalize(data), paste(filename, "-class.png", sep = ""));
    writePNG(array(pixels_class_0, c(size, size, 3)), paste(filename, "-class-0.png", sep = ""));
    writePNG(array(pixels_class_1, c(size, size, 3)), paste(filename, "-class-1.png", sep = ""));
    writePNG(array(pixels_class_2, c(size, size, 3)), paste(filename, "-class-2.png", sep = ""));
}