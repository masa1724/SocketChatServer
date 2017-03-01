########################################################################
# =  �P���W�J�ϐ�   �錾�����^�C�~���O�Œl����������
# := �ċA���W�J�ϐ� ���s���ɒl����������
# += �l���A�y���h����B 
#    �P���ɃA�y���h����̂ł͂Ȃ��A�擪�ɋ�(���p�X�y�[�X)�����Ă���l���A�y���h����B
# ��)
# HOGE=$(FUGA) �����s����FUGA����������
# FUGA=aa
# FUGA=bb
# all:
# 	@echo $(HOGE) �� aa bb���\�������(aa��bb�̊Ԃɋ󗓂��t������Ă���)
#
#------------------------------------------------------------------------
# �t�H���_�\����)
# MemSample(���[�N�X�y�[�X�z��)
#    |_ memmain.c
#    |_ samplefunc.c
#
# * : �}�b�`�������̂��X�y�[�X��؂�̕�����Ƃ��ēn�����
#  �擾�l = memmain.c samplefunc.c
#
# % : �}�b�`�������̂�1�����[�v�œn�����C���[�W
#  �擾�l = 1���=memmain.c 2���=samplefunc.c
#------------------------------------------------------------------------
#
# $@ : �^�[�Q�b�g��
# $^ : �^�[�Q�b�g�̈ˑ�����S�Ẵt�@�C�����擾
# $< : �^�[�Q�b�g�̈ˑ�����t�@�C���̍ŏ��̂��̂������擾����
#     ��) $^: memmain.c samplefunc.c   =>   $<: memmain.c
#
# $% : �^�[�Q�b�g���A�[�J�C�u�����o�������Ƃ��̃^�[�Q�b�g�����o��
# $? : �^�[�Q�b�g���V�������ׂĂ̈ˑ�����t�@�C����(�v�͈ˑ��t�@�C���̒��ōX�V���ꂽ���̂������擾����)
# $+ : Makefile�Ɠ������Ԃ̈ˑ�����t�@�C���̖��O
# $* : �^�[�Q�b�g������T�t�B�b�N�X�����������O
# 
# \ �����������s�ɑ����ꍇ�A������\��t������
# 
# - �R�}���h�̐擪�ɂ���ƃR�}���h���s���A�G���[�����������ꍇ�ł�����I�������ƌ��Ȃ��B
#   ����ɂ��A���������f���ꂸ�A�ȍ~�̃R�}���h�����s�����B
# @ �R�}���h�̐擪�ɂ���Ǝ��s�R�}���h��W���o�͂֕\������Ȃ��Ȃ�B
# 
# <�Q�l>
# https://www.ecoop.net/coop/translated/GNUMake3.77/make_toc.jp.html
# https://www.ecoop.net/coop/translated/GNUMake3.77/make_10.jp.html
# http://d.hatena.ne.jp/goth_wrist_cut/20080317/1205769293
# http://blog.wagavulin.jp/entry/20120405/1333629926
#
# �R���p�C�����
# [�S�̓I�ȃI�v�V����]
# -c    : �R���p�C���̂ݍs��(�I�u�W�F�N�g�t�@�C���𐶐�����)
# -o    : �R���p�C�������ۂɐ��������I�u�W�F�N�g�t�@�C���̏o�͐���w�肷��B
#         (�w�肵�Ȃ��ꍇ�A�J�����g�f�B���N�g����a.out�Ƃ������O�Ńt�@�C���o�͂���)
#
# [�x���I�v�V����]
# -Wall : �x���I�v�V���������ׂėL���ɂ���(http://at-aka.blogspot.jp/2006/11/gcc-wall-w.html)
# -W    : �x�����b�Z�[�W�����ڂ����\������
#         ���܃`�F�b�N���]�܂�鍀�ڂ͌x����\��������
# -fmessage-length=n : ���b�Z�[�W�̒������w��(n�𒴂����ꍇ�A�܂�Ԃ�)
#
# [�f�o�b�O�I�v�V����]
# -g    : ���s�t�@�C���Ƀf�o�b�O���𖄂ߍ���(gdb���Ŏg�p����)
#
# [�œK���I�v�V����]
# -O0   : �R���p�C�����A�R�[�h�̍œK�����s��Ȃ�
#          => �J�����͂��̃I�v�V������ݒ肷��
#             (�œK��������ƃR�[�h�̓���ւ���폜���s���Agdb�ł̃f�o�b�O������Ȃ�ׁB)
# -O2   : �R���p�C�����A�R�[�h�̍œK�����s��
#          => �{�Ԋ��K�p���͂��̃I�v�V������ݒ肷��
#             (�œK�����s�����Ƃŏ������x���コ���邽��)
#
# [�v���v���Z�b�T�I�v�V����]
# -M    : �I�u�W�F�N�g�Ԃ̈ˑ��֌W������B
# -MM   : �V�X�e���w�b�_�t�@�C���͏��O���ĕ\������B 
# -MD, MMD  : ��L�ɉ����A�R���p�C�����s���B(�����A-c���ݒ肵�Ȃ��Ƃ��߂݂���)
# -MF   : �ˑ��֌W��W���o�͂łȂ��A�w�肵���t�@�C���֏o�͂���
# -MP   : �ˑ�����w�b�_�t�@�C�����U�̃^�[�Q�b�g�Ƃ��Ēǉ�����B
########################################################################

# �^�[�Q�b�g��
TARGET=	SocketChatServer

# �f�o�b�O���[�h(��`���R�����g�A�E�g������Ɩ�����)
DEBUG=

# �R���p�C��
COMPILER=	gcc

# �R���p�C���I�v�V����
#  <�f�o�b�O���[�h�L����>
#    �E�R�[�h�̍œK������
#    �E�f�o�b�O���̖���
ifdef DEBUG
COMPILE_OPT_LINK=	-O2 -W -Wall -o
COMPILE_OPT=	-O2 -W -Wall -c
else
COMPILE_OPT_LINK=	-O0 -W -Wall -o
COMPILE_OPT=	-O0 -g3 -W -Wall -c
endif

# �\�[�X�t�@�C��
SRC_DIR= ./
SRCS=	$(wildcard $(SRC_DIR)*.c)

# �I�u�W�F�N�g�t�@�C��
OBJ_DIR=	./Debug/
OBJS=	$(addprefix $(OBJ_DIR),$(notdir $(SRCS:.c=.o)))

# �ˑ��֌W�t�@�C��
DEPS_DIR=	./Debug/depents/
DEPS=	$(addprefix $(DEPS_DIR),$(notdir $(SRCS:.c=.d)))

# ���s�t�@�C��
EXE=	$(OBJ_DIR)$(TARGET).exe


all:	$(TARGET)

# �I�u�W�F�N�g�t�@�C���������J���A���s�t�@�C���𐶐�����
$(TARGET): $(OBJS)
	$(COMPILER) $(COMPILE_OPT_LINK) $(EXE) $^ -l"ws2_32"


# �ec�t�@�C�����R���p�C�����A�I�u�W�F�N�g�t�@�C���𐶐�����
# �܂��e�I�u�W�F�N�g�t�@�C���̈ˑ��֌W���t�@�C���֏o�͂���
#
# $(OBJS): $(SRCS)              =>  ���� main.c memcfuncs2.c memfuncs.c���擾����� ���@�����炱�ꂾ�Ƃ���
# $(OBJ_DIR)%.o: $(SRC_DIR)%.c  =>  1��ځFmain.c 2��ځFmemcfuncs2.c 3��ځFmemfuncs.c
$(OBJ_DIR)%.o: $(SRC_DIR)%.c
	@[ -d $(OBJ_DIR) ] || mkdir $(OBJ_DIR)
	@[ -d $(DEPS_DIR) ] || mkdir $(DEPS_DIR)
	
	$(COMPILER) $(COMPILE_OPT) -MMD -MP -MF"$(DEPS_DIR)$(<F:.c=.d)" -MT"$(@)" -o $@ $<

clean:
	rm -f $(EXE) $(OBJS) $(DEPS)


# �ˑ��֌W�̒ǉ�
#  => ����ɂ��w�b�_�[�t�@�C���̂ݍX�V�����ꍇ�ł��A
#     �ˑ�����t�@�C�������R���p�C���E�����J�����
-include $(DEPS)

.PHONY: all clean $(TARGET)

