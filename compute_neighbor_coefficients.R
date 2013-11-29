compute_neighbor_coefficients = function(image, size, filename, outputFile) {
    right <- matrix(0, nrow = 3, ncol = 3);
    right[2, 2] <- 1;
    right[2, 1] <- -1;

    right_neighbor_differences <- filter2(image, right)[, 1:(size-1), ];
    right_neighbor_differences_norm <- right_neighbor_differences[,,1]^2 + right_neighbor_differences[,,1]^2 + 
                                       right_neighbor_differences[,,3]^2;

    down <- matrix(0, nrow = 3, ncol = 3);
    down[2, 2] <- 1;
    down[1, 2] <- -1;

    down_neighbor_differences <- filter2(image, down)[1:(size-1), , ];
    down_neighbor_differences_norm <- down_neighbor_differences[,,1]^2 + down_neighbor_differences[,,2]^2 + 
                                      down_neighbor_differences[,,3]^2;

    sigma <- 2*mean(c(right_neighbor_differences_norm, down_neighbor_differences));

    coefficients_right <- exp(-right_neighbor_differences_norm/(2*sigma^2));
    coefficients_down <- exp(-down_neighbor_differences_norm/(2*sigma^2));

    writePNG(normalize(matrix(coefficients_right, nrow = size, ncol = size-1)), paste(filename, "_coefficients_right.png", sep= ""));
    writePNG(normalize(matrix(coefficients_down, nrow = size-1, ncol = size)), paste(filename, "_coefficients_down.png", sep = ""));

    write_data_matrix_to_file(coefficients_right, "RIGHT", outputFile);
    write_data_matrix_to_file(coefficients_down, "DOWN", outputFile);
}