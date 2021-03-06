#pragma once
struct DynamicBoard {
	int cX;
	int cY;
	bool Board[4][4];
};
class CTetrisGame{
public:
	CTetrisGame(void);
	~CTetrisGame(void);
	void InitGame(void);//Khởi tạo game với các thông số mặc định 
	bool Drop(void);//Hạ khối gạch xuống 1 bậc, nếu không hạ được sẽ trả về 
	// giá trị false ,biến khối gạch động thành khối gạch thực,
	// lấy giá trị GenBoard và gọi hàm GenNext
	// yêu cầu hàm UpdateGame phải kiểm tra xem có ăn hay 
	// thua không.
	bool UpdateGame(void);//Hàm thức hiện drop, kiểm tra giá trị trả về của 
	// drop, nếu false sẽ cho thực hiện hàm CheckRow và CheckLose, giá trị 
	// trả về là true nếu thua và false nếu vẫn tiếp tục
	void CheckRow(void);//Kiểm tra từ dưới lên trên, nếu ăn thì tăng biến 
	// m_Together lên 1 đơn vị, gọi tiếp MoveRow và CheckRow. Nếu không ăn
	// thì gọi hàm cộng điểm, set m_Together = 0
	void MoveRow(int Row);//Hạ tất cả những khối gạch có chỉ số y < Row 
	// xuống 1 đơn vị, set hàng trên cùng = false all
	int AddScore(void);//Cộng điểm theo hệ số và cấp độ(Tốc độ)
	void Rotate(void);//Xoay khối Dynamic
	void MoveLeft(void);//Di chuyển khối gạch sang trái
	void MoveRight (void);//Di chuyển khối gạch sang phải
	bool MoveDown(void);//Di chuyển khối gạch xuống dưới bằng cách thực
	// hiện liên tục hàm Drop đến khi hàm trả false.
	int GetBoardCell (int x,int y);//Lấy giá trị ô x,y . Nếu là Dyn trả về
	// -1, nếu là blank trả về 0, nếu là gạch trả về 1.
	int ArrToInt (bool arr[4][4]);//Đổi giá trị mảng sang nguyên
	void IntToArr (bool arr[4][4],int i);//Đổi giá trị nguyên sang mảng
	bool CheckLose(void);//Kiểm tra thua
	void GenNext (void);//Tạo khối gạch mới
	bool GetNext(int x,int y);//Trả về giá trị của ô Next (true,false)
private:
	bool m_Board[10][22];// Mảng của bảng rộng 8 cao 22, thực chất chỉ hiện
	// từ 4 đến 21
	bool m_isPause;// Biến lưu giá trị có pause hay không
	DynamicBoard DynBoard;//Khối gạch đang lơ lửng
	DynamicBoard GenBoard;//Khối gạch ô Next
	int m_Difficulty;//Độ khó
	int m_Score;//Điểm
public:
	void SetDifficulty(int i);
	int GetDifficulty(void);
private:
	// Số lần ăn liên tiếp
	int m_Together;
public:
	// Tính 2 mũ i
	int LuyThua2(int i);
	void PauseGame(void);
	void ResumeGame(void);
	int GetScore(void);
};
