# https://learnopencv.com/blob-detection-using-opencv-python-c/
# https://dev.to/divshekhar/blob-detection-opencv-python-22li

# Standard imports
import cv2
import numpy as np;

# Read image
im = cv2.imread("SERGE 3PANEL KNOBS ONLY@5x.png", cv2.IMREAD_GRAYSCALE)

cv2.imshow("Knobs",im)
# cv2.waitKey(0)

# Setup SimpleBlobDetector parameters.
params = cv2.SimpleBlobDetector_Params()

params.filterByColor = True
params.blobColor = 0

# Change thresholds
params.minThreshold = 200
params.maxThreshold = 250 

# Filter by Area.
# params.filterByArea = True
# params.minArea = 1500

# Filter by Circularity
# params.filterByCircularity = True
# params.minCircularity = 0.1

# Filter by Convexity
# params.filterByConvexity = True
# params.minConvexity = 0.87

# Filter by Inertia
# params.filterByInertia = True
# params.minInertiaRatio = 0.01

# Create a detector with the parameters
ver = (cv2.__version__).split('.')
if int(ver[0]) < 3 :
	detector = cv2.SimpleBlobDetector(params)
else : 
	detector = cv2.SimpleBlobDetector_create(params)

# Detect blobs.
keypoints = detector.detect(im)

print(keypoints)
print(len(keypoints))
# Draw detected blobs as red circles.
# cv2.DRAW_MATCHES_FLAGS_DRAW_RICH_KEYPOINTS ensures the size of the circle corresponds to the size of blob
im_with_keypoints = cv2.drawKeypoints(im, keypoints, np.array([]), (0,0,255), cv2.DRAW_MATCHES_FLAGS_DRAW_RICH_KEYPOINTS)

# Show keypoints
cv2.imshow("Keypoints", im_with_keypoints)
cv2.waitKey(0)
