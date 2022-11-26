#include "seam_carver.hpp"
#include "image_ppm.hpp"

// implement the rest of SeamCarver's functions here

// given functions below, DO NOT MODIFY

SeamCarver::SeamCarver(const ImagePPM& image): image_(image) {
  height_ = image.GetHeight();
  width_ = image.GetWidth();
}

void SeamCarver::SetImage(const ImagePPM& image) {
  image_ = image;
  width_ = image.GetWidth();
  height_ = image.GetHeight();
}
const ImagePPM& SeamCarver::GetImage() const {
  return image_;
}
int SeamCarver::GetHeight() const {
  return height_;
}
int SeamCarver::GetWidth() const {
  return width_;
}
int SeamCarver::GetEnergy(int row, int col) const {
  Pixel left = image_.GetPixel(0,0);
  if (col == 0){
    left = image_.GetPixel(row, width_ - 1);
  } else {
    left = image_.GetPixel(row, col - 1);
  }

  Pixel right = image_.GetPixel(0,0);
  if (col == width_ - 1){
    right = image_.GetPixel(row, 0);
  } else {
    right = image_.GetPixel(row, col + 1);
  }

  Pixel up = image_.GetPixel(0,0);
  if (row == 0){
    up = image_.GetPixel(height_ - 1, col);
  } else {
    up = image_.GetPixel(row - 1, col);
  }

  Pixel down = image_.GetPixel(0,0);
  if (row == height_ -1){
    down = image_.GetPixel(0, col);
  } else {
    down = image_.GetPixel(row + 1, col);
  }

  return Change(left, right) + Change(up, down);
}
int SeamCarver::Change(Pixel first, Pixel second) const{
  return (first.GetBlue() - second.GetBlue()) * (first.GetBlue() - second.GetBlue()) +
         (first.GetGreen() - second.GetGreen()) * (first.GetGreen() - second.GetGreen()) +
         (first.GetRed() - second.GetRed()) * (first.GetRed() - second.GetRed());
}
int* SeamCarver::GetVerticalSeam() const {
  int** pathways = new int*[height_];
  for (int i = 0; i < height_; ++i){
    pathways[i] = new int[width_];
  }
  for (int j = 0; j < width_; ++j){
    pathways[height_ - 1][j] = GetEnergy(height_ - 1, j);
  }
  for (int row = height_ - 2; row >= 0; --row){
    for (int col = 0; col < width_; ++col){
      int middle = pathways[row + 1][col];
      if (col == 0){
        int right = pathways[row + 1][col + 1];
        if (right < middle){
          pathways[row][col] = GetEnergy(row, col) + right;
        } else {pathways[row][col] = GetEnergy(row, col) + middle;}
      } else if (col == width_ - 1){
        int left = pathways[row + 1][col - 1];
        if (left < middle){
          pathways[row][col] = GetEnergy(row, col) + left;
        } else {
          pathways[row][col] = GetEnergy(row, col) + middle;
        }
      } else {
        int left = pathways[row + 1][col - 1];
        int right = pathways[row + 1][col + 1];
        if (right < middle && right < left){
          pathways[row][col] = GetEnergy(row, col) + right;
        } else if (left < middle && left <= right){
          pathways[row][col] = GetEnergy(row, col) + left;
        } else {
          pathways[row][col] = GetEnergy(row, col) + middle;
        }
      }
    }
  }
  int* toreturn = FindVSeam(pathways);
  Delete(pathways);
  return toreturn;
}
int* SeamCarver::GetHorizontalSeam() const {
  int** pathways = new int*[height_];
  for (int i = 0; i < height_; ++i){
    pathways[i] = new int[width_];
  }
  for (int j = 0; j < height_; ++j){
    pathways[j][width_ - 1] = GetEnergy(j, width_ - 1);
  }
  for (int col = width_ - 2; col >= 0; --col){
    for (int row = 0; row < height_; ++row){
      int middle = pathways[row][col+1];
      if (row == 0){
        int down = pathways[row + 1][col + 1];
        if (down < middle){
          pathways[row][col] = GetEnergy(row, col) + down;
        } else {
          pathways[row][col] = GetEnergy(row, col) + middle;
        }
      } else if (row == height_ - 1){
        int up = pathways[row - 1][col + 1];
        if (up < middle){
          pathways[row][col] = GetEnergy(row, col) + up;
        } else {pathways[row][col] = GetEnergy(row, col) + middle;}
      } else {
        int down = pathways[row + 1][col + 1];
        int up = pathways[row - 1][col + 1];
        if (down < middle && down < up){
          pathways[row][col] = GetEnergy(row, col) + down;
        } else if (up < middle && up <= down){
          pathways[row][col] = GetEnergy(row, col) + up;
        } else {
          pathways[row][col] = GetEnergy(row, col) + middle;
        }
      }
    }
  }
  int* toreturn = FindHSeam(pathways);
  Delete(pathways);
  return toreturn;
}
int* SeamCarver::FindHSeam(int** pathways) const {
  int* toreturn = new int[width_];
  int small_row = 0;
  int min = pathways[0][0];
  for (int i = 0; i < height_; ++i){
    if (pathways[i][0] < min){
      small_row = i;
      min = pathways[i][0];
    }
  }
  toreturn[0] = small_row;
  for (int col = 0; col < width_ - 1; ++col){
    int right = pathways[small_row][col + 1];
    if (small_row == 0){
      int down = pathways[small_row + 1][col + 1];
      if (down < right){
        small_row = small_row + 1;
      }
    } else if (small_row == height_ - 1){
      int up = pathways[small_row - 1][col + 1];
      if (up < right){
        small_row = small_row - 1;
      }
    } else {
      int down = pathways[small_row + 1][col + 1];
      int up = pathways[small_row - 1][col + 1];
      if (down < right && down < up){
        small_row = small_row + 1;
      } else if (up < right && up <= down){
        small_row = small_row -1;
      }
    }
    toreturn[col + 1] = small_row;
  }
  return toreturn;
}
int* SeamCarver::FindVSeam(int** pathways) const{
  int* toreturn = new int[height_];
  int small_col = 0;
  int min_val = pathways[0][0];
  for (int i = 0; i < width_; ++i){
    if (pathways[0][i] < min_val ){
      small_col = i;
      min_val = pathways[0][i];
    }
  }
  toreturn[0] = small_col;
  for (int row = 0; row < height_ - 1; ++row){
    int down = pathways[row + 1][small_col];
    if (small_col == 0){
      int right = pathways[row + 1][small_col + 1];
      if (right < down){
        small_col = small_col + 1;
      }
    } else if (small_col == width_ - 1){
      int left = pathways[row + 1][small_col - 1];
      if (left < down){
        small_col = small_col - 1;
      }
    } else {
      int right = pathways[row + 1][small_col + 1];
      int left = pathways[row + 1][small_col - 1];
      if (right < left && right < down){
        small_col = small_col + 1;
      } else if (left <= right && left < down){
        small_col = small_col -1;
      }
    }
    toreturn[row + 1] = small_col;
  }
  return toreturn;
}
void SeamCarver::Delete(int** pathways) const {
  for (int i = 0; i < height_; ++i){
    delete[] pathways[i];
  }
  delete[] pathways;
}
void SeamCarver::RemoveVerticalSeam() {
  int* toskip = GetVerticalSeam();
  image_.VerticalHelper(toskip);
  delete[] toskip;
  width_--;
}
void SeamCarver::RemoveHorizontalSeam() {
  int* toskip = GetHorizontalSeam();
  image_.HorizontalHelper(toskip);
  delete[] toskip;
  height_--;
}