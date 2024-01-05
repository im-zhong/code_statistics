#!/usr/bin/env python
# -*- encoding: utf-8 -*-
'''
@File    :   file_process.py
@License :   (C)Copyright 2021 , atomsocial
'''

import csv
import os
import re
import zipfile

from Libs.error import APIParaError


class ZipFile:
    @staticmethod
    def unzip(file):
        """
        :param file:  io 或者文件名
        :return:
        """
        try:
            zfile = zipfile.ZipFile(file, 'r')
        except zipfile.BadZipFile as e:
            raise APIParaError(message="zip文件格式错误",data=str(e))
        files = []
        for i in zfile.filelist:
            try:
                filename=i.filename.encode('cp437').decode('gbk')
            except Exception:
                filename=i.filename
            files.append({
                "file_name":filename,
                "file_size":i.file_size,
                "content":zfile.read(i.filename),
                "suffix":filename.split(".")[-1]
            })
            # imghdr.what(None, h=zfile.read(i.filename)) # 判断是不是图片文件类型
        return files


class CSV:
    def __init__(self,encoding="utf8"):
        """
        bom-utf8 : 'utf-8-sig'
        - **encoding**:
        """
        self.encoding=encoding

    def load(self,path_name,header=True,output_type="dict"):
        '''
        return : {header:[],data:[]}  or None
        header: bool  , True 第一行是header
                        False 第一行就是数据
                list  , 直接放到result.header中
        output_type : "list"/dict
        '''
        with open(path_name, encoding=self.encoding) as csvfile:
            csvfile=(line.replace('\0','') for line in csvfile) # 杜绝nul相关的东西
            if output_type=="list":
                data=[]
                csv_reader = csv.reader(csvfile)  # 使用csv.reader读取csvfile中的文件
                if header:
                    header = next(csv_reader)  # 读取第一行每一列的标题
                else:
                    header=[]
                for row in csv_reader:  # 将csv 文件中的数据保存到birth_data中
                    data.append(row)
                return {"header":header,"data":data}
            else:
                data =list(csv.DictReader(csvfile))
                return {"header":self._get_header_acc(data),"data":data}
    def _get_header_acc(self,dict_datas):
        r=set()
        for item  in  dict_datas[:500]:
            r=r|set(item.keys())
        return list(r)
    def dump(self,path_name,data):
        '''
        data 的结构：{header:[], data:[]} 或者list // data[0]: list  dict
        return True/None
        '''
        with open(path_name, "w", encoding=self.encoding, newline="") as csv_file:
            if isinstance(data,list): #直接就是数据
                if isinstance(data[0],list):
                    #list write
                    writer = csv.writer(csv_file)
                    for item in data:
                        writer.writerow(item)
                else:
                    writer = csv.DictWriter(csv_file, self._get_header_acc(data))
                    writer.writeheader()
                    writer.writerows(data)

            elif isinstance(data,dict):
                data_row=data.get("data")
                header=data.get("header")
                if not data_row or not isinstance(data_row,list):
                    return
                if isinstance(data_row[0],list):
                    #list write
                    writer = csv.writer(csv_file)
                    if header:
                        writer.writerow(header)
                    for item in data_row:
                        writer.writerow(item)
                else:
                    if header:
                        writer = csv.DictWriter(csv_file, header)
                    else:
                        writer = csv.DictWriter(csv_file, self._get_header_acc(data_row))
                    writer.writeheader()
                    writer.writerows(data_row)
            return True

class File:
    @staticmethod
    def get_abs_path(para__file__: str, relative_path: str, sep=os.sep):
        """
        获取__file__ 的绝对路径。 需要提供目标文件距离__file__ 的相对路径relative_path
        这是段有格式的注释.
        >>> File().get_abs_path(r"C:\\Users\\EggBro\\Desktop\\前端\\atomsocial-frontend\\test","../../e/e/./s/../a.txt","/")
        'C:/Users/EggBro/Desktop/e/e/a.txt'
        >>> File().get_abs_path(r'''C:\\\\Users\\\\EggBro\\\\Desktop\\\\前端\\\\atomsocial-frontend\\\\test''',"../../e/e/./s/../a.txt","\\\\")
        'C:\\Users\\EggBro\\Desktop\\e\\e\\a.txt'

        :param para__file__:
        :param relative_path:  相对路径请使用 ./ 的形式
        :param sep:  希望输出的路径所携带的分隔符
        :return:
        """
        if relative_path.startswith("./") or relative_path.startswith("../"):
            paths = para__file__.split(os.sep)[:-1]
            path_point = len(paths) - 1
            for item in relative_path.split("../"):
                if item == "":
                    path_point -= 1
                else:  # 检测是否有./
                    paths = paths[:path_point + 1]
                    paths.append(item)
                    path_point += 1
            paths = paths[:path_point + 1]
            for index, item in enumerate(paths):  # 剔除每个 ./ 的内容
                item = sep.join(item.split("/./"))
                item = sep.join(item.split("./"))
                paths[index] = sep.join(item.split("/"))
            path = sep.join(paths)  # // 相当于 删除前一个节点

            def callback(matched):
                return sep

            if sep == "/":
                patten = "/(?P<name>\w+?)//"
            else:
                patten = r"\\(?P<name>\w+?)\\\\"
            replacedStr = re.sub(patten,
                                 callback, path)
            return replacedStr
        elif relative_path.startswith("/"):
            return relative_path
        else:  # 直接是文件开始的
            return sep.join(para__file__.split(sep)[:-1] + [relative_path])

