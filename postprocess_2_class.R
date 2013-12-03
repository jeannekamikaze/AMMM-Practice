 generate_2_class_images = function(filename) { 
    library(png);
    library(EBImage);
    
    data <- read.table(paste(filename, ".mat", sep = ""), sep = "\t");
    data <- apply(data, MARGIN = c(1,2), FUN = as.numeric)

    image <- readPNG(source = paste(filename, ".png", sep = ""), native = FALSE);

    channels <- 4;
    size <- sqrt(length(image) / channels);                

    image <- array(image, c(size, size, 4))[,,1:3];    

    pixels_foreground <- matrix(image, nrow = size*size, ncol = 3);
    pixels_background <- matrix(image, nrow = size*size, ncol = 3);

    pixels_foreground[which(data == 0), ] <- c(0, 0, 0);
    pixels_background[which(data == 1), ] <- c(0, 0, 0);

    writePNG(normalize(data), paste(filename, "-class.png", sep = ""));
    writePNG(array(pixels_foreground, c(size, size, 3)), paste(filename, "-foreground.png", sep = ""));
    writePNG(array(pixels_background, c(size, size, 3)), paste(filename, "-background.png", sep = ""));
}