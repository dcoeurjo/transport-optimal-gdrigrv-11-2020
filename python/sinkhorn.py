import numpy as np
import matplotlib.pyplot as plt
import matplotlib.image as mpimg

# minimum value of the gaussian
EPSILON = 1E-250

# blurring parameter
gamma = 50

# loads a grayscale image
img = mpimg.imread('evol1.bmp') 
img = img[:,:,0]


# defines the blurring operator "xi"
# using a separable gaussian with min value EPSILON
t = np.linspace(0, img.shape[0], img.shape[0])
[Y, X] = np.meshgrid(t, t)
xi1 = np.maximum(EPSILON, np.exp(-(X - Y) ** 2 / gamma))

t = np.linspace(0, img.shape[1], img.shape[1])
[Y, X] = np.meshgrid(t, t)
xi2 = np.maximum(EPSILON, np.exp(-(X - Y) ** 2 / gamma))

def xi(x):
  return np.dot(np.dot(xi1, x), xi2)

		
#apply the operator on the image and show the result
fig = plt.figure()
plt.gray()  # show the filtered result in grayscale
ax1 = fig.add_subplot(121)  # left side
ax2 = fig.add_subplot(122)  # right side

result = xi(img)
ax1.imshow(img)
ax2.imshow(result)

plt.show()