library(png);                                                                   #Reading and writting png files.
library(mclust);                                                                #EM clustering algorithm.

scribble <- readPNG(source = "scribble.png", native = FALSE);
image <- readPNG(source = "forest.png", native = FALSE);

channels <- 4;                                                                  #Assuming images are given in RGBA format.
size <- sqrt(length(scribble) / channels);                                      #Assuming square images.

scribble <- array(scribble, c(size, size, 4));                            
image <- array(image, c(size, size, 4))[,,1:3];                                 #Discarding alpha channel.

clusters <- 5                                                                   #Number of clusters to describe each class. (5 is the value used by the authors)
generate_representative = function(image, size, set_of_pixels_indices, clusters = 5) {
    columns <- (set_of_pixels_indices-1)%/%size+1;
    rows <- (set_of_pixels_indices-1)%%size+1;
    indexes <- cbind(rows, columns);

    scribbled_pixels <- cbind(image[cbind(indexes,1)],
                              image[cbind(indexes,2)],
                              image[cbind(indexes,3)]);
    
    subset <- sample(1:length(scribbled_pixels)/3, 1000, replace = FALSE)

    return(Mclust(data = scribbled_pixels[subset,], G = clusters)$parameters);
}

representatives_class_0 <- generate_representative(image, size, which(scribble[,,4] == 1  & scribble[,,1] == 1), clusters);
representatives_class_1 <- generate_representative(image, size, which(scribble[,,4] == 1  & scribble[,,2] == 1), clusters);

pixels <- matrix(image, nrow = size*size, ncol = 3);

distances_class_0 <- matrix(0, nrow = size*size, ncol = clusters);
distances_class_1 <- matrix(0, nrow = size*size, ncol = clusters);

for (i in 1:clusters) {
    distances_class_0[,i] <- mahalanobis(pixels, center = representatives_class_0$mean[1:3,i], cov = representatives_class_0$variance$sigma[,,i]);
    distances_class_1[,i] <- mahalanobis(pixels, center = representatives_class_1$mean[1:3,i], cov = representatives_class_1$variance$sigma[,,i]);
}

min_distances_class_0 <- apply(distances_class_0, 1, min);
min_distances_class_1 <- apply(distances_class_1, 1, min);


#writePNG(normalize(matrix(min_distances_class_1, nrow = 512, ncol = 512)), "distances_1");
#writePNG(normalize(matrix(min_distances_class_0, nrow = 512, ncol = 512)), "distances_0");
