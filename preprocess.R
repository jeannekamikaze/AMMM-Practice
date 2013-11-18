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

compute_distance_to_classes = function(image, scribble, size, filename) {
    clusters <- 5                                                                                                       #Number of clusters to describe each class. (5 is the value used by the authors)

    representatives_class_0 <- generate_representative(image, size, which(scribble[,,4] == 1  & scribble[,,1] == 1), 
        clusters);
    representatives_class_1 <- generate_representative(image, size, which(scribble[,,4] == 1  & scribble[,,2] == 1), 
        clusters);

    pixels <- matrix(image, nrow = size*size, ncol = 3);

    distances_class_0 <- matrix(0, nrow = size*size, ncol = clusters);
    distances_class_1 <- matrix(0, nrow = size*size, ncol = clusters);

    for (i in 1:clusters) {
        distances_class_0[,i] <- mahalanobis(pixels, center = representatives_class_0$mean[1:3,i], 
            cov = representatives_class_0$variance$sigma[,,i]);
        distances_class_1[,i] <- mahalanobis(pixels, center = representatives_class_1$mean[1:3,i], 
            cov = representatives_class_1$variance$sigma[,,i]);
    }

    min_distances_class_0 <- apply(distances_class_0, 1, min);
    min_distances_class_1 <- apply(distances_class_1, 1, min);

    writePNG(normalize(matrix(min_distances_class_1, nrow = 512, ncol = 512)), paste(filename, "_distances_1.png", sep = ""));
    writePNG(normalize(matrix(min_distances_class_0, nrow = 512, ncol = 512)), paste(filename, "_distances_0.png", sep = ""));
}

compute_neighbor_coefficients = function(image, size, filename) {
    right <- matrix(0, nrow = 3, ncol = 3);
    right[2, 2] <- 1;
    right[2, 3] <- -1;

    right_neighbor_differences <- filter2(image, right)[, 1:(size-1), ];
    right_neighbor_differences_norm <- right_neighbor_differences[,,1]^2 + right_neighbor_differences[,,1]^2 
                                       + right_neighbor_differences[,,3]^2;

    down <- matrix(0, nrow = 3, ncol = 3);
    down[2, 2] <- 1;
    down[3, 2] <- -1;

    down_neighbor_differences <- filter2(image, down)[1:(size-1), , ];
    down_neighbor_differences_norm <- down_neighbor_differences[,,1]^2 + down_neighbor_differences[,,2]^2 
                                      + down_neighbor_differences[,,3]^2;

    sigma <- 2*mean(c(right_neighbor_differences_norm, down_neighbor_differences_norm));

    coefficients_right <- exp(-right_neighbor_differences_norm/(sigma));
    coefficients_down <- exp(-down_neighbor_differences_norm/(sigma));

    writePNG(normalize(matrix(coefficients_right, nrow = 512, ncol = 511)), paste(filename, "_coefficients_right.png", sep= ""));
    writePNG(normalize(matrix(coefficients_down, nrow = 511, ncol = 512)), paste(filename, "_coefficients_down.png", sep = ""));
}

generate_labelling_coefficients = function(filename) {
    library(png);                                                                                                       #Reading and writting png files.
    library(mclust);                                                                                                    #EM clustering algorithm.
    library(EBImage);                                                                                                   #Operations over images. Filtering, normalizing, etc.

    scribble <- readPNG(source = paste(filename, "-scribble.png", sep = ""), native = FALSE);
    image <- readPNG(source = paste(filename, ".png", sep = ""), native = FALSE);

    channels <- 4;                                                                                                      #Assuming images are given in RGBA format.
    size <- sqrt(length(scribble) / channels);                                                                          #Assuming square images.

    scribble <- array(scribble, c(size, size, 4));                            
    image <- array(image, c(size, size, 4))[,,1:3];                                                                     #Discarding alpha channel.

    compute_distance_to_classes(image, scribble, size, filename);
    compute_neighbor_coefficients(image, size, filename);
}