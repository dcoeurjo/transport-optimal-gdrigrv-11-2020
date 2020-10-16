import numpy as np
import cv2

PATH="../"

# Load an color image
source = cv2.imread(PATH+'pexelA-0.png',cv2.IMREAD_COLOR)
print("Source image " + str(source.shape))

# Load an color image
target = cv2.imread(PATH+'pexelB-0.png',cv2.IMREAD_COLOR)
print("Target image " + str(target.shape))

cv2.imshow('source',source)
cv2.waitKey(0)
cv2.destroyAllWindows()

cv2.imshow('target',target)
cv2.waitKey(0)
cv2.destroyAllWindows()


##Swapping the channels
output = source
rows,cols,nbchannels = source.shape

for i in range(rows):
  for j in range(cols):
     color = source[i,j]
     color2 = [color[2], color[1], color[0]]
     output[i,j]=color2;

print("Saving the output")
cv2.imwrite('output.png',output)
