generate_labelling_coefficients = function(filename, number_of_classes = 2) {
    library(png);                                                                                                       #Reading and writting png files.
    library(mclust);                                                                                                    #EM clustering algorithm.
    library(EBImage);                                                                                                   #Operations over images. Filtering, normalizing, etc.
    library(colorspace);

    source(paste(getwd(), "/compute_distance_to_classes.R", sep = ""));
    source(paste(getwd(), "/compute_neighbor_coefficients.R", sep = ""));

    scribble <- readPNG(source = paste(filename, "-scribble.png", sep = ""), native = FALSE);
    image <- readPNG(source = paste(filename, ".png", sep = ""), native = FALSE);

    channels <- 4;                                                                                                      #Assuming images are given in RGBA format.
    size <- sqrt(length(scribble) / channels);                                                                          #Assuming square images.

    scribble <- array(scribble, c(size, size, 4));                            
    image <- array(coords(as(RGB(array(image,c(size*size,4))[,1:3]), "LUV")), c(size, size, 3));                                                                     #Discarding alpha channel.
    image[is.na(image)] <- 0;

    outputFile <- file(paste(filename, ".dat", sep = ""), "w");
    on.exit(close(outputFile));
    writeLines(c(paste("size = ", size, ";", sep = "")), outputFile);

    if (number_of_classes == 2) compute_distance_to_two_classes(image, scribble, size, filename, outputFile);
    if (number_of_classes == 3) compute_distance_to_three_classes(image, scribble, size, filename, outputFile);
    compute_neighbor_coefficients(image, size, filename, outputFile);
}

write_data_matrix_to_file = function(data, name, outputFile) {
    tmp <- apply(data,   1, function(x) { return(paste(x, collapse=" ")) });
    tmp <- apply(t(tmp), 2, function(x) { return(paste("[", x, "]", sep = "")) });
    writeLines(c(paste("\n", name, " = [", sep = ""), tmp, "];"), outputFile);
}