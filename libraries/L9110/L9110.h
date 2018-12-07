#ifndef L9110_h
#define L9110_h


class L9110 {
	private:
		int _aia;
		int _aib;
		int _bia;
		int _bib;
	public:
		L9110(int aia,int aib,int bia,int bib);
		void adelante(int i,int d,int t);
		void atras(int i,int d,int t);
		void izquierda(int i,int d,int t);
		void derecha(int i, int d,int t);
		void frenar(int t);
};

#endif
#define L9110_h