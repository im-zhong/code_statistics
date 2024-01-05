#!/usr/bin/env python
# -*- encoding: utf-8 -*-

'''
from xiaohan
这个是立场判断的一个服务

'''

class StanceEnum(str, Enum):
    against = 'AGAINST'
    favor = 'FAVOR'
    none = 'NONE'

if __name__ == "__main__":
    # 此程序只针对中文的处理方法
    print(Stance.get_sentence_stance("happy day"))  # FAVOR
    print(Stance.get_sentence_stance("快乐的一天"))  # None
    print(Stance.get_sentence_stance("你是不是傻"))  # None

''' ''' 

# 我懂了 第一次碰到' 我们就直接认为这是一个字符串了 
# 所以python的字符串判断需要额外进行处理 和block comment分开

from enum import Enum

from vaderSentiment.vaderSentiment import SentimentIntensityAnalyzer




    @staticmethod
    def to_name(value):
        for item in StanceEnum:
            if item.value == value:
                return item.name


class Stance:
    """
    Use VaderDict Method
    """
    analyzer = SentimentIntensityAnalyzer()
    lowerb = -0.05
    upperb = 0.05

    @classmethod
    def get_sentence_stance(cls, content: str) -> StanceEnum:
        vs = cls.analyzer.polarity_scores(content)
        compound = vs.pop('compound')
        stance = cls.__infer_stance_with_thresh(compound, lowerb=cls.lowerb, upperb=cls.upperb)
        return stance

    @staticmethod
    def __infer_stance_with_thresh(x, lowerb=-0.05, upperb=0.05) -> StanceEnum:
        if x < lowerb:
            stance = StanceEnum.against
        elif x > upperb:
            stance = StanceEnum.favor
        else:
            stance = StanceEnum.none
        return stance



