import cv2

def main():
    data_path = "./output"
    v_name = "texture.avi"
    fps = 20  # 视频帧率
    size = (640, 480)  # 需要转为视频的图片的尺寸
    video = cv2.VideoWriter(data_path+v_name, cv2.VideoWriter_fourcc('I', '4', '2', '0'), fps, size)

    for i in range(60):
        image_path = data_path + "frame" + str(i) + ".ppm"
        print(image_path)
        img = cv2.imread(image_path)
        video.write(img)

    video.release()
    cv2.destroyAllWindows()


if __name__ == "__main__":
    main()