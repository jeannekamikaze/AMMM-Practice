generate_representative = function(image, size, set_of_pixels_indices, clusters = 5) {
    columns <- (set_of_pixels_indices-1)%/%size+1;
    rows <- (set_of_pixels_indices-1)%%size+1;
    indexes <- cbind(rows, columns);

    scribbled_pixels <- cbind(image[cbind(indexes,1)],
                              image[cbind(indexes,2)],
                              image[cbind(indexes,3)]);
    
    subset <- sample(1:length(scribbled_pixels)/3, 1000, replace = FALSE);

    return(Mclust(data = scribbled_pixels[subset,], G = clusters)$parameters);
}

compute_distance_to_two_classes = function(image, scribble, size, filename, outputFile) {
    clusters <- 5;                                                                                                      #Number of clusters to describe each class. (5 is the value used by the authors)

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
    distances <- min_distances_class_1 - min_distances_class_0;

    distances <- (distances)/sd(distances);

    writePNG(normalize(matrix(min_distances_class_1, nrow = size, ncol = size)), paste(filename, "_distances_1.png", sep = ""));
    writePNG(normalize(matrix(min_distances_class_0, nrow = size, ncol = size)), paste(filename, "_distances_0.png", sep = ""));
    writePNG(normalize(matrix(distances, nrow = size, ncol = size)), paste(filename, "_distances.png", sep = ""));

    write_data_matrix_to_file(matrix(distances, nrow = size, ncol = size), "D", outputFile);
}

compute_distance_to_three_classes = function(image, scribble, size, filename, outputFile) {
    clusters <- 5;                                                                                                      #Number of clusters to describe each class. (5 is the value used by the authors)

    representatives_class_0 <- generate_representative(image, size, which(scribble[,,4] == 1  & scribble[,,1] == 1), 
        clusters);
    representatives_class_1 <- generate_representative(image, size, which(scribble[,,4] == 1  & scribble[,,2] == 1), 
        clusters);
    representatives_class_2 <- generate_representative(image, size, which(scribble[,,4] == 1  & scribble[,,3] == 1), 
        clusters);

    pixels <- matrix(image, nrow = size*size, ncol = 3);

    distances_class_0 <- matrix(0, nrow = size*size, ncol = clusters);
    distances_class_1 <- matrix(0, nrow = size*size, ncol = clusters);
    distances_class_2 <- matrix(0, nrow = size*size, ncol = clusters);

    for (i in 1:clusters) {
        distances_class_0[,i] <- mahalanobis(pixels, center = representatives_class_0$mean[1:3,i], 
            cov = representatives_class_0$variance$sigma[,,i]);
        distances_class_1[,i] <- mahalanobis(pixels, center = representatives_class_1$mean[1:3,i], 
            cov = representatives_class_1$variance$sigma[,,i]);
        distances_class_2[,i] <- mahalanobis(pixels, center = representatives_class_2$mean[1:3,i], 
            cov = representatives_class_2$variance$sigma[,,i]);
    }

    min_distances_class_0 <- apply(distances_class_0, 1, min);
    min_distances_class_1 <- apply(distances_class_1, 1, min);
    min_distances_class_2 <- apply(distances_class_2, 1, min);
    
    sigma <- sd(c(min_distances_class_0, min_distances_class_1, min_distances_class_2));

    min_distances_class_0 <- min_distances_class_0/sigma;
    min_distances_class_1 <- min_distances_class_1/sigma;
    min_distances_class_2 <- min_distances_class_2/sigma;

    writePNG(normalize(matrix(min_distances_class_0, nrow = size, ncol = size)), paste(filename, "_distances_0.png", sep = ""));
    writePNG(normalize(matrix(min_distances_class_1, nrow = size, ncol = size)), paste(filename, "_distances_1.png", sep = ""));
    writePNG(normalize(matrix(min_distances_class_2, nrow = size, ncol = size)), paste(filename, "_distances_2.png", sep = ""));

    write_data_matrix_to_file(matrix(min_distances_class_0, nrow = size, ncol = size), "D0", outputFile);
    write_data_matrix_to_file(matrix(min_distances_class_1, nrow = size, ncol = size), "D1", outputFile);
    write_data_matrix_to_file(matrix(min_distances_class_2, nrow = size, ncol = size), "D2", outputFile);
}