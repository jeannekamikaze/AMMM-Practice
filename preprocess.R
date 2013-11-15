library(png);                                                                   #Reading and writting png files.
library(mclust);                                                                #EM clustering algorithm.

scribble <- readPNG(source = "scribble.png", native = FALSE);
image <- readPNG(source = "forest.png", native = FALSE);

channels <- 4;                                                                  #Assuming images are given in RGBA format.
size <- sqrt(length(scribble) / channels);                                      #Assuming square images.

scribble <- array(scribble, c(size, size, 4))[,1:3];                            #Discarding alpha channel.
image <- array(image, c(size, size, 4));

generate_representative = function(image, size, set_of_pixels_indices) {
    columns <- (set_of_pixels_indices-1)%/%size+1;
    rows <- (set_of_pixels_indices-1)%%size+1;
    indexes <- cbind(rows, columns);

    scribbled_pixels <- cbind(image[cbind(indexes,1)],
                              image[cbind(indexes,2)],
                              image[cbind(indexes,3)]);

    return(Mclust(data = scribbled_pixels, G = 1)$parameters$mean);
}

representative_class0 <- generate_representative(image, size, which(scribble[,,4] == 1  & scribble[,,1] == 1));
representative_class1 <- generate_representative(image, size, which(scribble[,,4] == 1  & scribble[,,2] == 1));
